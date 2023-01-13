#include "Controller.hh"

#include <set>
#include <thread>

#include "state/State.hh"
#include "command/Command.hh"
#include "command/Commandable.hh"
#include "logger/Logger.hh"

namespace octopus
{
BufferedState::~BufferedState()
{
	delete _state;
	for(Step * step_l : _steps)
	{
		delete step_l;
	}
}

Controller::Controller(
	std::list<Steppable *> const &initSteppables_p,
	std::list<Command *> const &initCommands_p,
	double timePerStep_p)
	: _timePerStep(timePerStep_p)
{
	std::lock_guard<std::mutex> lock_l(_mutex);

	_backState = new BufferedState { 0, {}, new State() };
	_bufferState = new BufferedState { 0, {}, new State() };
	_frontState = new BufferedState { 0, {}, new State() };

	_ongoingStep = 1;

	// add steppable
	for(Steppable * steppable_l : initSteppables_p)
	{
		_initialStep.getSteppable().push_back(steppable_l);
	}
	// init step (does not matter which state we use here since they are identical)
	for(Command *cmd_l : initCommands_p)
	{
		cmd_l->applyCommand(_initialStep, *_backState->_state);
	}

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
}


bool Controller::loop_body()
{
	bool upToDate_l = true;
	// go as far as handling the step before the ongoing one
	if(_backState->_stepHandled < _ongoingStep - 1)
	{
		upToDate_l = false;
		Logger::getDebug() << "step back state on step "<< _backState->_stepHandled << " " <<_backState->_state<< std::endl;
		// increment iterator to step
		_backState->_steps.push_back(new Step());
		// increment number of step hadled
		++_backState->_stepHandled;

		// if step was not handled already
		Logger::getDebug() << "handle commands" << " "<<_backState->_state<< std::endl;

		std::list<Command *> * commands_l = nullptr;
		{
			// lock to avoid overlap
			std::lock_guard<std::mutex> lock_l(_mutex);
			commands_l = _commitedCommands.at(_backState->_stepHandled-1);
		}

		// push new commands
		for(Command * cmd_l : *commands_l)
		{
			Logger::getDebug() << "\tregister a command" << " "<<_backState->_state<< std::endl;
			cmd_l->registerCommand(*_backState->_state);
		}

		// apply all commands
		for(Commandable * cmdable_l : _backState->_state->getCommandables())
		{
			cmdable_l->runCommands(*_backState->_steps.back(), *_backState->_state);
		}

		Logger::getDebug() << "apply step" << " "<<_backState->_state<< std::endl;
		// apply step
		apply(*_backState->_steps.back(), *_backState->_state);

		// update last handled step
		Logger::getDebug() << "last handled step = " << _backState->_stepHandled << " "<<_backState->_state<< std::endl;
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

/// @brief commit commands on the ongoing step
void Controller::commitCommand(Command * command_p)
{
	// lock to avoid swap during commiting
	std::lock_guard<std::mutex> lock_l(_mutex);

	updateCommitedCommand();
	_commitedCommands.at(_ongoingStep-1)->push_back(command_p);
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
		_commitedCommands.push_back(new std::list<Command *>());
	}
}

} // namespace octopus
