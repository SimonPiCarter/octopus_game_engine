#include "Controller.hh"

#include <set>
#include <thread>

#include "state/State.hh"
#include "step/Step.hh"
#include "command/Command.hh"
#include "logger/Logger.hh"

namespace octopus
{
BufferedState::~BufferedState()
{
	delete _state;
}

Controller::Controller(
	std::list<Steppable *> const &initSteppables_p,
	std::list<Command *> const &initCommands_p,
	double timePerStep_p)
	: _timePerStep(timePerStep_p)
{
	std::lock_guard<std::mutex> lock_l(_mutex);

	_steps.push_back(new Step());
	_backState = new BufferedState { 1, _steps.begin(), new State() };
	_bufferState = new BufferedState { 1, _steps.begin(), new State() };
	_frontState = new BufferedState { 1, _steps.begin(), new State() };

	_lastHandledStep = 1;
	_ongoingStep = 2;

	// add steppable
	for(Steppable * steppable_l : initSteppables_p)
	{
		_steps.back()->getSteppable().push_back(steppable_l);
	}
	// init step (does not matter which state we use here since they are identical)
	for(Command *cmd_l : initCommands_p)
	{
		cmd_l->registerCommand(*_steps.back(), *_backState->_state);
	}

	// apply step
	apply(*_steps.back(), *_backState->_state);
	apply(*_steps.back(), *_bufferState->_state);
	apply(*_steps.back(), *_frontState->_state);
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
	std::set<Command const *> setCommand_l;
	for(std::list<Command *> const &cmds_l : _commitedCommands)
	{
		for(Command const *cmd_l : cmds_l)
		{
			setCommand_l.insert(cmd_l);
		}
	}
	for(Command const *cmd_l : setCommand_l)
	{
		delete cmd_l;
	}
}


void Controller::loop_body()
{
	// go as far as handling the step before the ongoing one
	if(_backState->_stepHandled < _ongoingStep - 1)
	{
		Logger::getDebug() << "step back state" << std::endl;
		// increment iterator to step
		++_backState->_it;
		// increment number of step hadled
		++_backState->_stepHandled;

		// if step was not handled already
		if(_backState->_stepHandled > _lastHandledStep)
		{
			Logger::getDebug() << "handle commands" << std::endl;
			{
				// lock to avoid swap during commiting
				std::lock_guard<std::mutex> lock_l(_mutex);

				updateCommitedCommand();
			}
			for(Command * cmd_l : _commitedCommands[_backState->_stepHandled-1])
			{
				Logger::getDebug() << "\thandling a command" << std::endl;
				// if command is not over propagate it to the next step
				if(!cmd_l->registerCommand(**_backState->_it, *_backState->_state))
				{
					// lock to avoid swap during commiting
					std::lock_guard<std::mutex> lock_l(_mutex);
					_commitedCommands.at(_backState->_stepHandled).push_back(cmd_l);
				}
			}
		}

		Logger::getDebug() << "apply step" << std::endl;
		// apply step
		apply(**_backState->_it, *_backState->_state);

		// update last handled step
		_lastHandledStep = std::max(_lastHandledStep, _backState->_stepHandled);
		Logger::getDebug() << "last handled step = " << _lastHandledStep << std::endl;
	}

	{
		// lock to avoid multi swap
		std::lock_guard<std::mutex> lock_l(_mutex);
		if(_backState->_stepHandled > _bufferState->_stepHandled)
		{
			Logger::getDebug() << "swap state" << std::endl;
			std::swap(_bufferState, _backState);
		}
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
	while (_overTime >= _timePerStep)
	{
		_overTime -= _timePerStep;
		++_ongoingStep;
		_steps.push_back(new Step());
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

/// @brief commit commands on the ongoing step
void Controller::commitCommand(Command * command_p)
{
	// lock to avoid swap during commiting
	std::lock_guard<std::mutex> lock_l(_mutex);

	updateCommitedCommand();
	_commitedCommands.at(_ongoingStep-1).push_back(command_p);
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

void Controller::updateCommitedCommand()
{
	while(_commitedCommands.size() < _ongoingStep)
	{
		_commitedCommands.push_back(std::list<Command *>());
	}
}

} // namespace octopus
