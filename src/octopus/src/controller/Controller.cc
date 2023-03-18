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
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandPopStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "orca/OrcaManager.hh"

namespace octopus
{

BufferedState::BufferedState(unsigned long stepHandled_p, std::list<Step *>::iterator it_p, State *state_p)
	: _stepHandled(stepHandled_p), _stepIt(it_p), _state(state_p)
{}

BufferedState::~BufferedState()
{
	delete _state;
}

unsigned long long BufferedState::getNextStepToApplyId() const
{
	return (*_stepIt)->getId();
}

Controller::Controller(
	std::list<Steppable *> const &initSteppables_p,
	double timePerStep_p,
	std::list<Command *> const &initCommands_p,
	unsigned long gridPointSize_p,
	unsigned long gridSize_p)
	: _timePerStep(timePerStep_p)
	, _initialStep(nullptr)
{
	std::lock_guard<std::mutex> lock_l(_mutex);

	_ongoingStep = 1;
	updateCommitedCommand();
	_lastHandledStep = 0;
	_compiledSteps.push_back(new Step(&_initialStep));

	_backState = new BufferedState { 0, _compiledSteps.begin(), new State(0, gridSize_p, gridPointSize_p) };
	_bufferState = new BufferedState { 0, _compiledSteps.begin(), new State(1, gridSize_p, gridPointSize_p) };
	_frontState = new BufferedState { 0, _compiledSteps.begin(), new State(2, gridSize_p, gridPointSize_p) };

	_commitedCommands.push_back(new std::list<Command *>());
	_triggers.push_back(std::list<Trigger const *>());
	_commands.push_back(std::list<AbstractCommand const *>());
	// add steppable
	for(Steppable * steppable_l : initSteppables_p)
	{
		CommandSpawnStep * cmdSpawn_l = dynamic_cast<CommandSpawnStep *>(steppable_l);
		if(cmdSpawn_l)
		{
			_commitedCommands[0]->push_back(cmdSpawn_l->getCmd());
		}
		TriggerSpawn * triggerSpawn_l = dynamic_cast<TriggerSpawn *>(steppable_l);
		if(triggerSpawn_l)
		{
			_triggers[0].push_back(triggerSpawn_l->_trigger);
		}
		_initialStep.addSteppable(steppable_l);
	}
	// add commands
	for(Command * cmd_l : initCommands_p)
	{
		_commitedCommands[0]->push_back(cmd_l);
		cmd_l->registerCommand(_initialStep, *_backState->_state);
	}

	octopus::updateStepFromConflictPosition(_initialStep, *_backState->_state);
	octopus::compact(_initialStep);

	for(SteppableBundle &bundle_l : _initialStep.getSteppable())
	{
		Steppable const * steppable_l = bundle_l._steppable;
		// store commands for memory handling
		CommandSpawnStep const * cmdSpawn_l = dynamic_cast<CommandSpawnStep const *>(steppable_l);
		CommandStorageStep const * cmdstorage_l = dynamic_cast<CommandStorageStep const *>(steppable_l);
		FlyingCommandSpawnStep const * flyingCmdSpawn_l = dynamic_cast<FlyingCommandSpawnStep const *>(steppable_l);

		if(cmdSpawn_l)
		{
			_commands[0].push_back(cmdSpawn_l->getCmd());
		}
		if(cmdstorage_l)
		{
			_commands[0].push_back(cmdstorage_l->getCmd());
		}
		if(flyingCmdSpawn_l)
		{
			_commands[0].push_back(flyingCmdSpawn_l->getCmd());
		}
	}

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
	delete _orcaManager;
	for(std::list<Command *> const *cmds_l : _commitedCommands)
	{
		delete cmds_l;
	}
	for(std::list<AbstractCommand const *> const &cmds_l : _commands)
	{
		for(AbstractCommand const * cmd_l : cmds_l)
		{
			delete cmd_l;
		}
	}
	for(std::list<Trigger const *> const &triggers_l : _triggers)
	{
		for(Trigger const *trigger_l : triggers_l)
		{
			delete trigger_l;
		}
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

			for(auto &&pair_l : _backState->_state->getFlyingCommands())
			{
				FlyingCommandBundle const & cmd_l = pair_l.second;
				// if over remove it
				if(cmd_l._cmd->applyCommand(step_l, *_backState->_state, cmd_l._data, _pathManager))
				{
					step_l.addSteppable(new FlyingCommandPopStep(cmd_l._cmd));
				}
			}

			// push new commands
			std::list<Command *> * commands_l = nullptr;
			{
				// lock to avoid overlap
				std::lock_guard<std::mutex> lock_l(_mutex);
				commands_l = _commitedCommands.at(_backState->_stepHandled);
			}
			for(Command * cmd_l : *commands_l)
			{
				cmd_l->registerCommand(step_l, *_backState->_state);
			}

			Logger::getDebug() << "processing step " << _backState->_stepHandled << " on state "<<_backState->_state->_id<< std::endl;

			if(_orcaCollision)
			{
				// test should reset based on last bacause we want to update the manager on the current state
				// therefore we need to check on the step just applied
				if(OrcaManager::ShouldReset(_orcaManager, *_backState->_state, *step_l.getPrev()))
				{
					delete _orcaManager;
					_orcaManager = new OrcaManager(1, 35., 10, 10., 10.);
					_orcaManager->resetFromState(*_backState->_state);
					_orcaManager->setupStep(*_backState->_state, step_l);
					_orcaManager->doStep();
					_orcaManager->commitStep(*_backState->_state, step_l);
				}
				else
				{
					_orcaManager->setupStep(*_backState->_state, step_l);
					_orcaManager->doStep();
					_orcaManager->commitStep(*_backState->_state, step_l);
				}
			}
			else
			{
				for(size_t i = 0; i < 1 && octopus::updateStepFromConflictPosition(step_l, *_backState->_state) ; ++ i) {}
			}

			// push new triggers
			for(Trigger * trigger_l : _queuedTriggers[_backState->_stepHandled-1])
			{
				step_l.addSteppable(new TriggerSpawn(trigger_l));
			}
			handleTriggers(*_backState->_state, step_l, getStepBeforeLastCompiledStep());

			octopus::compact(step_l);

			// register all commands

			for(SteppableBundle &bundle_l : step_l.getSteppable())
			{
				Steppable const * steppable_l = bundle_l._steppable;
				// store commands for memory handling
				CommandSpawnStep const * cmdSpawn_l = dynamic_cast<CommandSpawnStep const *>(steppable_l);
				CommandStorageStep const * cmdstorage_l = dynamic_cast<CommandStorageStep const *>(steppable_l);
				FlyingCommandSpawnStep const * flyingCmdSpawn_l = dynamic_cast<FlyingCommandSpawnStep const *>(steppable_l);
				if(cmdSpawn_l)
				{
					_commands[_backState->_stepHandled].push_back(cmdSpawn_l->getCmd());
				}
				if(cmdstorage_l)
				{
					_commands[_backState->_stepHandled].push_back(cmdstorage_l->getCmd());
				}
				if(flyingCmdSpawn_l)
				{
					_commands[_backState->_stepHandled].push_back(flyingCmdSpawn_l->getCmd());
				}
				TriggerSpawn const * triggerSpawn_l = dynamic_cast<TriggerSpawn const *>(steppable_l);
				if(triggerSpawn_l)
				{
					_triggers[_backState->_stepHandled].push_back(triggerSpawn_l->_trigger);
				}
			}

			// compute paths (update grid)
			_pathManager.initFromGrid(_backState->_state->getPathGrid().getInternalGrid(), _backState->_state->getPathGridStatus());
			_pathManager.startCompute(5000);

			// Prepare next step
			_compiledSteps.push_back(new Step(_compiledSteps.back()));
			_compiledSteps.back()->addSteppable(new TickingStep());
			_lastHandledStep = _backState->_stepHandled;

			auto &&time_l = std::chrono::nanoseconds( std::chrono::steady_clock::now() - start_l ).count();
			_metrics._nbStepsCompiled += 1;
			_metrics._timeCompilingSteps += time_l;
			if(time_l > _timePerStep * 1e9)
			{
				++_metrics._spikeCompilingSteps;
			}
			if(time_l > _metrics._maxTimeCompilingSteps)
			{
				_metrics._maxTimeCompilingSteps = time_l;
			}
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

	unsigned long long minId_l = _backState->getNextStepToApplyId();
	{
		// lock to avoid overlap
		std::lock_guard<std::mutex> lock_l(_mutex);
		minId_l = std::min(minId_l, _frontState->getNextStepToApplyId());
		minId_l = std::min(minId_l, _bufferState->getNextStepToApplyId());
	}
	// free steps if necessary
	while(_compiledSteps.size() > 100 && _compiledSteps.front()->getId() < minId_l)
	{
		delete _compiledSteps.front();
		_compiledSteps.pop_front();
	}
	return upToDate_l;
}

/// @brief increment ongoing step if necessary
/// increment back buffer for as much as required
/// perform internal swaps if necessary
unsigned long Controller::update(double elapsedTime_p)
{
	unsigned long steps_l = 0;
	// lock to avoid overlap
	std::lock_guard<std::mutex> lock_l(_mutex);

	// update on going step
	_overTime += elapsedTime_p;
	while (_overTime >= _timePerStep)
	{
		++steps_l;
		_overTime -= _timePerStep;
		++_ongoingStep;
		updateCommitedCommand();
	}
	return steps_l;
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
	_commitedCommands[_ongoingStep]->push_back(cmd_p);
}

void Controller::commitCommandAsPlayer(Command * cmd_p, unsigned long player_p)
{
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	if(cmd_p->checkPlayer(*_frontState->_state, player_p))
	{
		_commitedCommands[_ongoingStep]->push_back(cmd_p);
	}
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
	while(_commitedCommands.size() <= _ongoingStep)
	{
		_commitedCommands.push_back(new std::list<Command *>());
		_triggers.push_back(std::list<Trigger const *>());
		_commands.push_back(std::list<AbstractCommand const *>());
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
