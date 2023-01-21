#include "Controller.hh"

#include <set>
#include <thread>
#include <ctime>
#include <chrono>

#include "state/State.hh"
#include "command/Command.hh"
#include "command/Commandable.hh"
#include "logger/Logger.hh"

#include "step/ConflictPositionSolver.hh"
#include "step/TickingStep.hh"

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
	double timePerStep_p)
	: _timePerStep(timePerStep_p)
{
	std::lock_guard<std::mutex> lock_l(_mutex);

	_ongoingStep = 1;
	_lastHandledStep = 0;
	_compiledSteps.push_back(new Step());

	_backState = new BufferedState { 0, _compiledSteps.begin(), new State(0) };
	_bufferState = new BufferedState { 0, _compiledSteps.begin(), new State(1) };
	_frontState = new BufferedState { 0, _compiledSteps.begin(), new State(2) };

	// add steppable
	for(Steppable * steppable_l : initSteppables_p)
	{
		_initialStep.getSteppable().push_back(steppable_l);
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
	delete _backState;
	delete _bufferState;
	delete _frontState;
	std::set<Command const *> setCommand_l;
	for(std::list<Command *> const *cmds_l : _commitedCommands)
	{
		for(Command const *cmd_l : *cmds_l)
		{
			setCommand_l.insert(cmd_l);
		}

		delete cmds_l;
	}
	for(Command const *cmd_l : setCommand_l)
	{
		delete cmd_l;
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
		Logger::getDebug() << "step back state on step "<< _backState->_stepHandled << " " <<_backState->_state<< std::endl;
		// increment number of step hadled
		++_backState->_stepHandled;

		// next step has not been compiled : we need to compile commands into the step
		if(std::next(_backState->_stepIt) == _compiledSteps.end())
		{
			// Compile current step
			Step &step_l = *_compiledSteps.back();

			// if step was not handled already
			Logger::getDebug() << "compiling step " << _backState->_stepHandled << " on state "<<_backState->_state<< std::endl;

			const std::chrono::time_point<std::chrono::steady_clock> start_l = std::chrono::steady_clock::now();

			// apply all commands
			for(Commandable * cmdable_l : _backState->_state->getCommandables())
			{
				cmdable_l->runCommands(step_l, *_backState->_state);
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
				Logger::getDebug() << "\tregister a command" << " "<<_backState->_state<< std::endl;
				cmd_l->registerCommand(step_l);
			}

			Logger::getDebug() << "processing step " << _backState->_stepHandled << " on state "<<_backState->_state<< std::endl;

			for(size_t i = 0; i < 5 && octopus::updateStepFromConflictPosition(step_l, *_backState->_state) ; ++ i) {}
			octopus::compact(step_l);

			// Prepare next step
			_compiledSteps.push_back(new Step());
			_compiledSteps.back()->addSteppable(new TickingStep());
			_lastHandledStep = _backState->_stepHandled;

			_metrics._nbStepsCompiled += 1;
			_metrics._timeCompilingSteps += std::chrono::nanoseconds( std::chrono::steady_clock::now() - start_l ).count();
		}

		Logger::getDebug() << "apply step" << " "<<_backState->_state<< std::endl;

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
		Logger::getDebug() << "last handled step = " << _backState->_stepHandled << " for state "<<_backState->_state<< std::endl;
	}

	{
		// lock to avoid multi swap
		std::lock_guard<std::mutex> lock_l(_mutex);
		if(_backState->_stepHandled > _bufferState->_stepHandled)
		{
			Logger::getDebug() << "swap state" << " "<<_backState->_state<< std::endl;
			std::swap(_bufferState, _backState);
			upToDate_l = false;
		}
	}
	Logger::getDebug() << "up to date " << " "<<upToDate_l<< std::endl;
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
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);

	if(_bufferState->_stepHandled > _frontState->_stepHandled)
	{
		std::swap(_bufferState, _frontState);
	}
	return _frontState->_state;
}

StateAndSteps Controller::queryStateAndSteps()
{
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);

	if(_bufferState->_stepHandled > _frontState->_stepHandled)
	{
		std::swap(_bufferState, _frontState);
	}
	return { _frontState->_stepIt, _compiledSteps , _frontState->_state, _initialStep };
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

void Controller::updateCommitedCommand()
{
	while(_commitedCommands.size() < _ongoingStep)
	{
		_commitedCommands.push_back(new std::list<Command *>());
	}
}

} // namespace octopus
