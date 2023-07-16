#include "UnitModel.hh"

#include "logger/Logger.hh"

#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuffCommand.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

#include "orca/rvo/KdTree.hh"
#include "orca/rvo/Agent.hh"

namespace octopus
{

UnitModel::UnitModel(bool isStatic_p, Fixed ray_p, Fixed stepSpeed_p, Fixed hpMax_p)
	: EntityModel(isStatic_p, ray_p, stepSpeed_p, hpMax_p)
{
	_idleFunc = unitIdleFunction;
	_isUnit = true;
}

void registerUnit(std::vector<std::pair<octopus::Fixed , const Entity *> > &neighbors_p, bool sameTeam_p, size_t max_p, State const &state_p,
	RVO::Agent *agentFrom_p, RVO::Agent const *agentTo_p, octopus::Fixed &distance_p)
{
	Entity const * source_l = agentFrom_p->getEntity();
	Entity const * target_l = agentTo_p->getEntity();
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	bool teamCheck_l = (sameTeam_p && team_l == state_p.getPlayer(target_l->_player)->_team)
					|| (!sameTeam_p && team_l != state_p.getPlayer(target_l->_player)->_team);

	if (agentFrom_p != agentTo_p && teamCheck_l)
	{
		const octopus::Fixed distSq_l = absSq(agentFrom_p->getPosition() - agentTo_p->getPosition());

		if (distSq_l < distance_p) {
			if (neighbors_p.size() < max_p) {
				neighbors_p.push_back(std::make_pair(distSq_l, agentTo_p->getEntity()));
			}

			size_t i = neighbors_p.size() - 1;

			while (i != 0 && distSq_l < neighbors_p[i - 1].first) {
				std::swap(neighbors_p[i], neighbors_p[i - 1]);
				--i;
			}

			neighbors_p[i] = std::make_pair(distSq_l, agentTo_p->getEntity());

			if (neighbors_p.size() == max_p) {
				distance_p = neighbors_p.back().first;
			}
		}
	}
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
				// check for target
				Entity const * target_l = lookUpNewBuffTarget(state_p, unit_l._handle, unit_l._unitModel._buffer._range, unit_l._unitModel._buffer._buff);
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
	if(context_p.kdTree)
	{
		if(ent_p._model._damage > 1e-3 || ent_p._model._heal > 1e-3)
		{
			Logger::getDebug() << " Unit::runCommands :: no command (attack)"<< std::endl;
			if(ent_p._waiting >= waitingTimeForAttackScan_p)
			{
				std::vector<std::pair<octopus::Fixed , const Entity *> > neighbors_l;
				Fixed matchDistance_p = ent_p._aggroDistance;
				context_p.kdTree->computeEntityNeighbors(ent_p._handle, matchDistance_p,
					std::bind(registerUnit, std::ref(neighbors_l), ent_p._model._heal > 1e-3, 1, std::ref(state_p),
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				if(!neighbors_l.empty())
				{
					Entity const * target_l = neighbors_l.begin()->second;
					if(target_l)
					{
						Logger::getDebug() << " Unit::runCommands :: add attack command" << ent_p._handle << " -> " << target_l->_handle << std::endl;
						return new EntityAttackCommand(ent_p._commandableHandle, ent_p._handle, target_l->_handle, false);
					}
				}
			}
		}
	}
	else if(ent_p._model._damage > 1e-3 || ent_p._model._heal > 1e-3)
	{
		Logger::getDebug() << " Unit::runCommands :: no command (attack)"<< std::endl;
		if(ent_p._waiting >= waitingTimeForAttackScan_p)
		{
			Entity const * target_l = lookUpNewTarget(state_p, ent_p._handle, ent_p._aggroDistance, ent_p._model._heal > 1e-3);
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

