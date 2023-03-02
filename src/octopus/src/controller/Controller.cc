#include "Controller.hh"

#include <set>
#include <thread>
#include <ctime>
#include <chrono>

#include "command/Command.hh"
#include "command/Commandable.hh"
#include "controller/event/EventCollection.hh"
#include "controller/trigger/Listener.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/ConflictPositionSolver.hh"
#include "step/TickingStep.hh"
#include "step/trigger/TriggerEnableChange.hh"
#include "step/trigger/TriggerSpawn.hh"

namespace octopus
{

BufferedState::BufferedState(unsigned long stepHandled_p, std::list<Step *>::iterator it_p, State *state_p)
	: _stepHandled(stepHandled_p), _stepIt(it_p), _state(state_p)
{}

BufferedState::~BufferedState()
{
	delete _state;
}

Controller::Controller(
	std::list<Steppable *> const &initSteppables_p,
	double timePerStep_p,
	std::list<Command *> const &initCommands_p,
	unsigned long gridSize_p)
	: _timePerStep(timePerStep_p)
{
	std::lock_guard<std::mutex> lock_l(_mutex);

	_ongoingStep = 1;
	updateCommitedCommand();
	_lastHandledStep = 0;
	_compiledSteps.push_back(new Step());

	_backState = new BufferedState { 0, _compiledSteps.begin(), new State(0, gridSize_p) };
	_bufferState = new BufferedState { 0, _compiledSteps.begin(), new State(1, gridSize_p) };
	_frontState = new BufferedState { 0, _compiledSteps.begin(), new State(2, gridSize_p) };

	// add steppable
	for(Steppable * steppable_l : initSteppables_p)
	{
		_initialStep.addSteppable(steppable_l);
	}
	// add commands
	for(Command * cmd_l : initCommands_p)
	{
		cmd_l->registerCommand(_initialStep, *_backState->_state);
	}

	octopus::updateStepFromConflictPosition(_initialStep, *_backState->_state);
	octopus::compact(_initialStep);

	// apply step
	apply(_initialStep, *_backState->_state);
	apply(_initialStep, *_bufferState->_state);
	apply(_initialStep, *_frontState->_state);
}

Controller::~Controller()
{
	_pathManager.joinCompute();
	delete _backState;
	delete _bufferState;
	delete _frontState;
	for(std::list<Command *> const *cmds_l : _commitedCommands)
	{
		delete cmds_l;
	}
	for(Step * step_l : _compiledSteps)
	{
		delete step_l;
	}
}


bool Controller::loop_body()
{
	bool upToDate_l = true;
	// go as far as handling the step before the ongoing one
	if(_backState->_stepHandled < _ongoingStep - 1)
	{
		upToDate_l = false;
		Logger::getDebug() << "step back state on step "<< _backState->_stepHandled << " " <<_backState->_state->_id<< std::endl;
		// increment number of step hadled
		++_backState->_stepHandled;

		// next step has not been compiled : we need to compile commands into the step
		if(std::next(_backState->_stepIt) == _compiledSteps.end())
		{
			// Compile current step
			Step &step_l = *_compiledSteps.back();

			// if step was not handled already
			Logger::getDebug() << "compiling step " << _backState->_stepHandled << " on state "<<_backState->_state->_id<< std::endl;

			const std::chrono::time_point<std::chrono::steady_clock> start_l = std::chrono::steady_clock::now();

			_pathManager.joinCompute();

			// apply all commands
			for(Commandable * cmdable_l : _backState->_state->getEntities())
			{
				if(cmdable_l->isActive())
				{
					cmdable_l->runCommands(step_l, *_backState->_state, _pathManager);
				}
			}

			// push new commands
			std::list<Command *> * commands_l = nullptr;
			{
				// lock to avoid overlap
				std::lock_guard<std::mutex> lock_l(_mutex);
				commands_l = _commitedCommands.at(_backState->_stepHandled-1);
			}
			for(Command * cmd_l : *commands_l)
			{
				cmd_l->registerCommand(step_l, *_backState->_state);
			}

			Logger::getDebug() << "processing step " << _backState->_stepHandled << " on state "<<_backState->_state->_id<< std::endl;

			for(size_t i = 0; i < 1 && octopus::updateStepFromConflictPosition(step_l, *_backState->_state) ; ++ i) {}

			// push new triggers
			{
				// lock to avoid overlap
				std::lock_guard<std::mutex> lock_l(_mutex);

				for(Trigger * trigger_l : _queuedTriggers[_backState->_stepHandled-1])
				{
					step_l.addSteppable(new TriggerSpawn(trigger_l));
				}
			}
			handleTriggers(*_backState->_state, step_l, getStepBeforeLastCompiledStep());

			octopus::compact(step_l);

			// compute paths (update grid)
			_pathManager.initFromGrid(_backState->_state->getPathGrid().getInternalGrid(), _backState->_state->getPathGridStatus());
			_pathManager.startCompute(5000);

			// Prepare next step
			_compiledSteps.push_back(new Step());
			_compiledSteps.back()->addSteppable(new TickingStep());
			_lastHandledStep = _backState->_stepHandled;

			_metrics._nbStepsCompiled += 1;
			_metrics._timeCompilingSteps += std::chrono::nanoseconds( std::chrono::steady_clock::now() - start_l ).count();
		}

		Logger::getDebug() << "apply step" << " "<<_backState->_state->_id<< std::endl;

		const std::chrono::time_point<std::chrono::steady_clock> start_l = std::chrono::steady_clock::now();

		// apply step
		apply(**_backState->_stepIt, *_backState->_state);
		// increment iterator
		++_backState->_stepIt;

		// update metrics
		updateStateMetrics(_metrics, *_backState->_state);
		_metrics._nbStepsApplied += 1;
		_metrics._timeApplyingSteps += std::chrono::nanoseconds( std::chrono::steady_clock::now() - start_l ).count();

		// update last handled step
		Logger::getDebug() << "last handled step = " << _backState->_stepHandled << " for state "<<_backState->_state->_id<< std::endl;
	}

	{
		// lock to avoid multi swap
		std::lock_guard<std::mutex> lock_l(_mutex);
		if(_backState->_stepHandled > _bufferState->_stepHandled)
		{
			Logger::getDebug() << "swap state" << " "<<_backState->_state->_id<< std::endl;
			std::swap(_bufferState, _backState);
			upToDate_l = false;
		}
	}
	return upToDate_l;
}

/// @brief increment ongoing step if necessary
/// increment back buffer for as much as required
/// perform internal swaps if necessary
void Controller::update(double elapsedTime_p)
{
	// lock to avoid overlap
	std::lock_guard<std::mutex> lock_l(_mutex);

	// update on going step
	_overTime += elapsedTime_p;
	while (_overTime >= _timePerStep)
	{
		_overTime -= _timePerStep;
		++_ongoingStep;
		updateCommitedCommand();
	}
}

/// @brief update and return the front state
/// only swap the buffer and front state if buffer state is more advanced than front state
State const * Controller::queryState()
{

	if(_bufferState->_stepHandled > _frontState->_stepHandled)
	{
		// lock to avoid multi swap
		std::lock_guard<std::mutex> lock_l(_mutex);
		std::swap(_bufferState, _frontState);
	}
	return _frontState->_state;
}

StateAndSteps Controller::queryStateAndSteps()
{

	if(_bufferState->_stepHandled > _frontState->_stepHandled)
	{
		// lock to avoid multi swap
		std::lock_guard<std::mutex> lock_l(_mutex);
		std::swap(_bufferState, _frontState);
	}
	return { _frontState->_stepIt, _compiledSteps , _frontState->_state, _initialStep };
}

void Controller::commitCommand(Command * cmd_p)
{
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	_commitedCommands[_ongoingStep-1]->push_back(cmd_p);
}

/// @brief add a trigger on the ongoing step
void Controller::commitTrigger(Trigger * trigger_p)
{
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	_queuedTriggers[_ongoingStep-1].push_back(trigger_p);
}

State const * Controller::getBackState() const
{
	return _backState->_state;
}
State const * Controller::getBufferState() const
{
	return _bufferState->_state;
}
State const * Controller::getFrontState() const
{
	return _frontState->_state;
}

Metrics const &Controller::getMetrics() const
{
	return _metrics;
}

unsigned long Controller::getOngoingStep() const
{
	return _ongoingStep-1;
}

void Controller::updateCommitedCommand()
{
	while(_commitedCommands.size() < _ongoingStep)
	{
		_commitedCommands.push_back(new std::list<Command *>());
	}
}

void Controller::handleTriggers(State const &state_p, Step &step_p, Step const &prevStep_p)
{
	Logger::getDebug() << "handleTriggers :: start "<<std::endl;
	// compute event and triggers (visit last step)
	EventCollection visitor_l(state_p);
	visitAll(prevStep_p, visitor_l);

	Handle curHandle_l = 0;
	for(Trigger const * trigger_l : state_p.getTriggers())
	{
		TriggerData const &data_l = *state_p.getTriggerData(curHandle_l);
		if(!data_l._isEnabled)
		{
			Logger::getDebug() << "handleTriggers :: is disabled "<<curHandle_l<<std::endl;
			++curHandle_l;
			continue;
		}
		if(trigger_l->isComplete(data_l))
		{
			// trigger
			for(size_t count_l = 0 ; count_l < trigger_l->getCount(data_l) ; ++count_l)
			{
				Logger::getDebug() << "handleTriggers :: trigger on trigger "<<curHandle_l<<std::endl;
				trigger_l->trigger(state_p, step_p, count_l);
			}

			// handle reset or disabling
			if(trigger_l->_isOneShot)
			{
				// if one shot disable trigger
				step_p.addSteppable(new TriggerEnableChange(curHandle_l, data_l._isEnabled, false));
				Logger::getDebug() << "handleTriggers :: disable "<<curHandle_l<<std::endl;
			}
			else
			{
				// Else reset data
				trigger_l->reset(step_p, data_l);
				Logger::getDebug() << "handleTriggers :: reset "<<curHandle_l<<std::endl;
			}
		}
		// update trigger/listener states
		trigger_l->compile(visitor_l, step_p, data_l);
		Logger::getDebug() << "handleTriggers :: compile "<<curHandle_l<<std::endl;

		++curHandle_l;
	}
}

Step const & Controller::getStepBeforeLastCompiledStep() const
{
	if(_compiledSteps.empty()
	|| _compiledSteps.back() == _compiledSteps.front())
	{
		return _initialStep;
	}
	auto &&rit_l = _compiledSteps.rbegin();
	++rit_l;
	return **rit_l;
}

} // namespace octopus
