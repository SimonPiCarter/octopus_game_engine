#include "Controller.hh"

#include <thread>

#include "state/State.hh"
#include "step/Step.hh"
#include "command/Command.hh"

namespace octopus
{
BufferedState::~BufferedState()
{
	delete _state;
}

Controller::Controller(Step * initialStep_p, double timePerStep_p) : _timePerStep(timePerStep_p)
{
	std::lock_guard<std::mutex> lock_l(_mutex);

	_steps.push_back(initialStep_p);
	_backState = new BufferedState { 1, _steps.begin(), new State() };
	_bufferState = new BufferedState { 1, _steps.begin(), new State() };
	_frontState = new BufferedState { 1, _steps.begin(), new State() };

	// apply step
	apply(*initialStep_p, *_backState->_state);
	apply(*initialStep_p, *_bufferState->_state);
	apply(*initialStep_p, *_frontState->_state);
}

Controller::~Controller()
{
	delete _backState;
	delete _bufferState;
	delete _frontState;
	for(Step * step_l : _steps)
	{
		delete step_l;
	}
	for(std::list<Command *> const &cmds_l : _commitedCommands)
	{
		for(Command const *cmd_l : cmds_l)
		{
			delete cmd_l;
		}
	}
}


void Controller::loop_body()
{
	{
		// lock to avoid multi swap
		std::lock_guard<std::mutex> lock_l(_mutex);
		if(_backState->_stepHandled > _bufferState->_stepHandled)
		{
			std::swap(_bufferState, _backState);
		}
	}

	// go as far as handling the step before the ongoing one
	if(_backState->_stepHandled < _ongoingStep - 1)
	{
		// increment iterator to step
		++_backState->_it;
		// increment number of step handled
		++_backState->_stepHandled;

		// apply step
		apply(**_backState->_it, *_backState->_state);
	}
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
	while (_overTime > _timePerStep)
	{
		_overTime -= _timePerStep;
		++_ongoingStep;
	}
}

/// @brief update and return the front state
/// only swap the buffer and front state if buffer state is more advanced than front state
BufferedState * Controller::queryState()
{
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);

	if(_bufferState->_stepHandled > _frontState->_stepHandled)
	{
		std::swap(_bufferState, _frontState);
	}
	return _frontState;
}

/// @brief commit commands on the ongoing step
void Controller::commitCommand(Command * command_p)
{
	// lock to avoid swap during commiting
	std::lock_guard<std::mutex> lock_l(_mutex);

	while(_commitedCommands.size() <= _ongoingStep)
	{
		_commitedCommands.push_back(std::list<Command *>());
	}

	_commitedCommands.at(_ongoingStep).push_back(command_p);
}

} // namespace octopus
