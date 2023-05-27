#include "Controller.hh"

#include <set>
#include <thread>
#include <ctime>
#include <chrono>

#include "command/Command.hh"
#include "command/Commandable.hh"
#include "controller/event/EventCollection.hh"
#include "controller/event/EventEntityModelDied.hh"
#include "controller/event/EventEntityModelFinished.hh"
#include "controller/trigger/Listener.hh"
#include "logger/Logger.hh"
#include "orca/OrcaManager.hh"
#include "serialization/CommandSerialization.hh"
#include "state/State.hh"
#include "step/ConstraintPosition.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandPopStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/ConflictPositionSolver.hh"
#include "step/TickingStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerProducedUpgradeStep.hh"
#include "step/player/PlayerUpdateBuildingCountStep.hh"
#include "step/trigger/TriggerEnableChange.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "step/vision/VisionChangeStep.hh"


namespace octopus
{

BufferedState::BufferedState(unsigned long stepHandled_p, std::list<StepBundle>::iterator it_p, State *state_p)
	: _stepHandled(stepHandled_p), _stepIt(it_p), _state(state_p)
{}

BufferedState::~BufferedState()
{
	delete _state;
}

unsigned long long BufferedState::getNextStepToApplyId() const
{
	return _stepIt->_step->getId();
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
	_stepBundles.push_back({new Step(&_initialStep), new StepData()});

	_backState = new BufferedState { 0, _stepBundles.begin(), new State(0, gridSize_p, gridPointSize_p) };
	_bufferState = new BufferedState { 0, _stepBundles.begin(), new State(1, gridSize_p, gridPointSize_p) };
	_frontState = new BufferedState { 0, _stepBundles.begin(), new State(2, gridSize_p, gridPointSize_p) };

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
		cmd_l->setStepOfRegristration(_initialStep.getId());
		cmd_l->registerCommand(_initialStep, *_backState->_state);
	}

	octopus::updateStepFromConflictPosition(_initialStep, *_backState->_state);
	octopus::compact(_initialStep);

	for(Steppable const *steppable_l : _initialStep.getSteppable())
	{
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
	apply(_initialStep, *_backState->_state, _initialStepData);
	apply(_initialStep, *_bufferState->_state, _initialStepData);
	apply(_initialStep, *_frontState->_state, _initialStepData);
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
	for(std::list<Command *> const *cmds_l : _queuedCommands)
	{
		for(Command const * cmd_l : *cmds_l)
		{
			delete cmd_l;
		}
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
	for(StepBundle &step_l : _stepBundles)
	{
		delete step_l._stepData;
		delete step_l._step;
	}
}


bool Controller::loop_body()
{
	bool upToDate_l = true;
	// go as far as handling the step before the ongoing one
	if(_backState->_stepHandled < _ongoingStep - 1)
	{
		upToDate_l = false;
		State *state_l = _backState->_state;
		Logger::getDebug() << "step back state on step "<< _backState->_stepHandled << " " <<state_l->_id<< std::endl;
		// increment number of step hadled
		++_backState->_stepHandled;

		// next step has not been compiled : we need to compile commands into the step
		if(std::next(_backState->_stepIt) == _stepBundles.end())
		{
			// Compile current step
			Step &step_l = *_stepBundles.back()._step;

			// if step was not handled already
			Logger::getDebug() << "compiling step " << _backState->_stepHandled << " on state "<<state_l->_id<< std::endl;

			const std::chrono::time_point<std::chrono::steady_clock> start_l = std::chrono::steady_clock::now();

			_pathManager.joinCompute();

			// apply all commands
			for(Commandable * cmdable_l : state_l->getEntities())
			{
				if(cmdable_l->isActive())
				{
					cmdable_l->runCommands(step_l, *state_l, _pathManager);
				}
			}

			for(auto &&pair_l : state_l->getFlyingCommands())
			{
				FlyingCommandBundle const & cmd_l = pair_l.second;
				// if over remove it
				if(cmd_l._cmd->applyCommand(step_l, *state_l, cmd_l._data, _pathManager))
				{
					step_l.addSteppable(new FlyingCommandPopStep(cmd_l._cmd));
				}
			}

			// push new commands
			std::list<Command *> * commands_l = nullptr;
			{
				// lock to avoid overlap
				//std::lock_guard<std::mutex> lock_l(_mutex);
				commands_l = _commitedCommands.at(_backState->_stepHandled);
			}
			for(Command * cmd_l : *commands_l)
			{
				cmd_l->setStepOfRegristration(step_l.getId());
				cmd_l->registerCommand(step_l, *state_l);
			}

			Logger::getDebug() << "processing step " << _backState->_stepHandled << " on state "<<state_l->_id<< std::endl;

			if(_orcaCollision)
			{
				// test should reset based on last bacause we want to update the manager on the current state
				// therefore we need to check on the step just applied
				if(OrcaManager::ShouldReset(_orcaManager, *state_l, *step_l.getPrev()))
				{
					delete _orcaManager;
					_orcaManager = new OrcaManager(1, 5., 10, 10., 10.);
					_orcaManager->resetFromState(*state_l);
					_orcaManager->setupStep(*state_l, step_l);
					_orcaManager->doStep();
					_orcaManager->commitStep(*state_l, step_l);
				}
				else
				{
					_orcaManager->setupStep(*state_l, step_l);
					_orcaManager->doStep();
					_orcaManager->commitStep(*state_l, step_l);
				}
			}
			else
			{
				for(size_t i = 0; i < 1 && octopus::updateStepFromConflictPosition(step_l, *state_l) ; ++ i) {}
			}

			octopus::updateStepFromConstraintPosition(step_l, *state_l, state_l->getConstraintPositionLibrary());

			// push new triggers
			for(Trigger * trigger_l : _queuedTriggers[_backState->_stepHandled-1])
			{
				step_l.addSteppable(new TriggerSpawn(trigger_l));
			}
			handleTriggers(*state_l, step_l, getStepBeforeLastCompiledStep());

			std::list<VisionChangeStep *> list_l = newVisionChangeStep(*state_l, step_l, state_l->getWorldSize(), state_l->getVisionHandler().getPatternHandler());
			std::for_each(list_l.begin(), list_l.end(), std::bind(&Step::addSteppable, &step_l, std::placeholders::_1));

			octopus::compact(step_l);

			// register all commands

			for(Steppable const * steppable_l : step_l.getSteppable())
			{
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
			_pathManager.initFromGrid(state_l->getPathGrid().getInternalGrid(), state_l->getPathGridStatus());
			_pathManager.startCompute(5000);

			// Prepare next step
			_stepBundles.push_back({new Step(_stepBundles.back()._step), new StepData()});
			_stepBundles.back()._step->addSteppable(new TickingStep());
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

		Logger::getDebug() << "apply step" << " "<<state_l->_id<< std::endl;

		const std::chrono::time_point<std::chrono::steady_clock> start_l = std::chrono::steady_clock::now();

		// apply step
		apply(*(_backState->_stepIt->_step), *state_l, *(_backState->_stepIt->_stepData));
		// increment iterator
		++_backState->_stepIt;

		// update metrics
		updateStateMetrics(_metrics, *state_l);
		_metrics._nbStepsApplied += 1;
		_metrics._timeApplyingSteps += std::chrono::nanoseconds( std::chrono::steady_clock::now() - start_l ).count();

		// update last handled step
		Logger::getDebug() << "last handled step = " << _backState->_stepHandled << " for state "<<state_l->_id<< std::endl;
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
	while(_stepBundles.size() > 100 && _stepBundles.front()._step->getId() < minId_l)
	{
		delete _stepBundles.front()._stepData;
		delete _stepBundles.front()._step;
		_stepBundles.pop_front();
	}
	return upToDate_l;
}

/// @brief increment ongoing step if necessary
/// increment back buffer for as much as required
/// perform internal swaps if necessary
unsigned long Controller::update(double elapsedTime_p)
{
	unsigned long steps_l = 0;

	// In replay mode et cant go farther than steps loaded
	if(_replayMode
	&& _commitedCommands.size() == _ongoingStep+1)
	{
		return steps_l;
	}

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
	return { _frontState->_stepIt, _stepBundles , _frontState->_state, _initialStep };
}

void Controller::commitCommand(Command * cmd_p)
{
	if(_replayMode)
	{
		return;
	}
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	_commitedCommands[_ongoingStep]->push_back(cmd_p);
}

void Controller::commitCommandAsPlayer(Command * cmd_p, unsigned long player_p)
{
	if(_replayMode)
	{
		return;
	}
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	if(cmd_p->checkPlayer(*_frontState->_state, player_p))
	{
		_commitedCommands[_ongoingStep]->push_back(cmd_p);
	}
}

void Controller::queueCommandAsPlayer(Command * cmd_p, unsigned long player_p)
{
	if(_replayMode)
	{
		return;
	}
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	if(!_queuedCommands.empty() && cmd_p->checkPlayer(*_frontState->_state, player_p))
	{
		_queuedCommands.back()->push_back(cmd_p);
	}
}

void Controller::addQueuedLayer()
{
	// lock to avoid multi swap
	std::lock_guard<std::mutex> lock_l(_mutex);
	_queuedCommands.push_back(new std::list<Command *>());
}

size_t Controller::getQueuedSize() const
{
	return _queuedCommands.size();
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

void Controller::setOngoingStep(unsigned long step_p)
{
	_ongoingStep = step_p;
	// update containers
	updateCommitedCommand();
}

void Controller::setReplayMode(bool replayMode_p)
{
	_replayMode = replayMode_p;
}

void Controller::setOnlineSaveFile(std::ofstream* of_p)
{
	_of = of_p;
}

void Controller::setOnlineSaveFileDebug(std::ofstream* of_p)
{
	_ofDebug = of_p;
}

void Controller::updateCommitedCommand()
{
	while(_commitedCommands.size() <= _ongoingStep)
	{
		// online save if necessary
		if(_of && !_commitedCommands.back()->empty())
		{
			writeListOfCommand(*_of, _commitedCommands.back(), _commitedCommands.size()-1);
		}
		if(_ofDebug && !_commitedCommands.back()->empty())
		{
			writeDebugListOfCommand(*_ofDebug, _commitedCommands.back(), _commitedCommands.size()-1);
		}
		if(_queuedCommands.empty())
		{
			_commitedCommands.push_back(new std::list<Command *>());
		}
		else
		{
			// use queued command
			_commitedCommands.push_back(_queuedCommands.front());
			_queuedCommands.pop_front();
		}
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

	// handle update for every player of building counts using a map of delta (+1 means a new building)
	std::map<unsigned long, std::map<std::string, long> > mapDeltaBuildingPerPlayer_l;

	// check every unit destroyed to cancel any command with ProductionData and refund
	for(EventEntityModelDied const * died_l : visitor_l._listEventEntityModelDied)
	{
		Entity const & ent_l = died_l->_entity;
		if(died_l->_model._isBuilding && ent_l.getQueue().hasCommand())
		{
			// inspect running commands
			for(CommandBundle const &bundle_l : ent_l.getQueue().getList())
			{
				ProductionData const *data_l = dynamic_cast<ProductionData const *>(getData(bundle_l._var));
				// refund cost of unit production
				if(data_l)
				{
					step_p.addSteppable(new PlayerSpendResourceStep(ent_l._player, getReverseCostMap(data_l->getCost())));
				}
				// if update remove update being processed
				UpgradeProductionData const * upgradeData_l = dynamic_cast<UpgradeProductionData const *>(data_l);
				if(upgradeData_l)
				{
					step_p.addSteppable(new PlayerProducedUpgradeStep(ent_l._player, upgradeData_l->_upgrade->_id, false));
				}
			}
		}
		// decrement building count for the given player
		if(died_l->_model._isBuilding)
		{
			mapDeltaBuildingPerPlayer_l[ent_l._player][ent_l._model._id] -= 1;
		}
	}

	for(EventEntityModelFinished const * spanwed_l : visitor_l._listEventEntityModelFinished)
	{
		Entity const & ent_l = spanwed_l->_entity;
		// increment building count for the given player
		if(ent_l._model._isBuilding)
		{
			mapDeltaBuildingPerPlayer_l[ent_l._player][ent_l._model._id] += 1;
		}
	}

	for(auto &&pair_l : mapDeltaBuildingPerPlayer_l)
	{
		step_p.addSteppable(new PlayerUpdateBuildingCountStep(pair_l.first, pair_l.second));
	}

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
				trigger_l->trigger(state_p, step_p, count_l, data_l);
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
	if(_stepBundles.empty()
	|| _stepBundles.back()._step == _stepBundles.front()._step)
	{
		return _initialStep;
	}
	auto &&rit_l = _stepBundles.rbegin();
	++rit_l;
	return *rit_l->_step;
}

} // namespace octopus
