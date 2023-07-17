#include "UnitModel.hh"

#include "logger/Logger.hh"

#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuffCommand.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

#include "utils/KdTree.hh"

namespace octopus
{

UnitModel::UnitModel(bool isStatic_p, Fixed ray_p, Fixed stepSpeed_p, Fixed hpMax_p)
	: EntityModel(isStatic_p, ray_p, stepSpeed_p, hpMax_p)
{
	_idleFunc = unitIdleFunction;
	_isUnit = true;
}

Command * commandFromIdle(Entity const &ent_p, State const &state_p, unsigned long waitingTimeForAttackScan_p, CommandContext &context_p)
{
	// if no command and buff we check for target
	if(ent_p._model._isUnit)
	{
		Unit const &unit_l = static_cast<Unit const &>(ent_p);
		if(unit_l._unitModel._buffer._active)
		{
			Logger::getDebug() << " Unit::runCommands :: no command (buff)"<< std::endl;

			if(unit_l._waiting+1 >= unit_l._unitModel._buffer._reload)
			{
				std::vector<std::pair<Fixed, Entity const *> > neighbors_l = context_p.kdTree->computeEntityNeighbors(
					unit_l._handle, unit_l._unitModel._buffer._range, 0, std::bind(
						teamCheck, state_p.getPlayer(ent_p._player)->_team,
						!unit_l._unitModel._buffer._buff._debuff, std::ref(state_p), std::placeholders::_1));


				Entity const * target_l = nullptr;
				unsigned long timeSinceBuff_l = 0;
				for(auto &&pair_l : neighbors_l)
				{
					Entity const * ent_l = pair_l.second;
					if(unit_l._unitModel._buffer._buff.isApplying(state_p, ent_p, *ent_l))
					{
						unsigned long curTimeSinceBuff_l = ent_l->getTimeSinceBuff(unit_l._unitModel._buffer._buff._id);
						// update target if no target yet
						// if time since buff is longer
						if(target_l == nullptr
						|| curTimeSinceBuff_l > timeSinceBuff_l)
						{
							target_l = ent_l;
							timeSinceBuff_l = curTimeSinceBuff_l;
						}
					}
				}

				if(target_l)
				{
					// buff
					Logger::getDebug() << " Unit::runCommands :: add buff command" << unit_l._handle << " -> " << target_l->_handle <<"("<<unit_l._unitModel._buffer._buff._id<<")"<< std::endl;
					return new EntityBuffCommand(unit_l._commandableHandle, target_l->_handle, unit_l._unitModel._buffer._buff);
				}
			}
		}
	}

	// If no command we check for target if we have damage or heal
	if(ent_p._model._damage > 1e-3 || ent_p._model._heal > 1e-3)
	{
		Logger::getDebug() << " Unit::runCommands :: no command (attack)"<< std::endl;
		if(ent_p._waiting >= waitingTimeForAttackScan_p)
		{
			Entity const * target_l = getClosestEntity(*context_p.kdTree, ent_p._handle, ent_p._aggroDistance,
				std::bind(teamCheck, state_p.getPlayer(ent_p._player)->_team, ent_p._model._heal > 1e-3, std::ref(state_p), std::placeholders::_1));
			if(target_l)
			{
				Logger::getDebug() << " Unit::runCommands :: add attack command" << ent_p._handle << " -> " << target_l->_handle << std::endl;
				return new EntityAttackCommand(ent_p._commandableHandle, ent_p._handle, target_l->_handle, false);
			}
		}
	}

	return nullptr;
}

void unitIdleFunction(Entity const &ent_p, Step & step_p, State const &state_p, CommandContext &context_p)
{
	static unsigned long const waitTime_l = 50;
	// check for attack every 50 steps
	Command * cmd_l = commandFromIdle(ent_p, state_p, waitTime_l, context_p);

	if(cmd_l)
	{
		// reset waiting time
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
		step_p.addSteppable(new CommandSpawnStep(cmd_l));
	}
	else if(ent_p._waiting >= waitTime_l)
	{
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
	}
}

} // namespace octopus

