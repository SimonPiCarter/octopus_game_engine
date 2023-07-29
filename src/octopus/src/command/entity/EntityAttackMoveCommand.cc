
#include "EntityAttackMoveCommand.hh"

#include <cmath>
#include "EntityAttackCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandSetPositionFromStep.hh"
#include "step/command/CommandNewTargetStep.hh"
#include "step/command/CommandSubAttackStep.hh"
#include "step/entity/EntityAttackStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityAttackMoveCommand::EntityAttackMoveCommand(Handle const &commandHandle_p, Handle const &source_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p, bool neverStop_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _subMoveCommand(commandHandle_p, source_p, finalPoint_p, gridStatus_p, waypoints_p, init_p, neverStop_p)
	, _data(Handle(0), _subMoveCommand.getFinalPoint(), _subMoveCommand.getGridStatus(), _subMoveCommand.getWaypoints())
{}

void EntityAttackMoveCommand::setFlockInformation(FlockInformation * flockInfo_p)
{
	_data._flockInfo = flockInfo_p;
}

bool shouldStopAttacking(AttackMoveData const &attackMoveData_p, Entity const *ent_p)
{
	/// distance when to stop running after target (squared)
	Fixed maxSqDistance_l = 25.;
	Vector diff_l = ent_p->_pos - attackMoveData_p._positionFromAttack;
	return square_length(diff_l) > maxSqDistance_l;
}

bool EntityAttackMoveCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const
{
	Logger::getDebug() << "EntityAttackMoveCommand:: apply Command "<<_source <<std::endl;
	AttackMoveData const &attackMoveData_l = *static_cast<AttackMoveData const *>(data_p);
	EntityAttackCommand const & subAttackCommand_l = attackMoveData_l._subAttackCommand;
	EntityBuffCommand const & subBuffCommand_l = attackMoveData_l._subBuffCommand;
	Entity const * ent_l = state_p.getEntity(_source);

	if(attackMoveData_l._hasSubAttackCommand)
	{
		if(!subAttackCommand_l.applyCommand(step_p, state_p, data_p, pathManager_p))
		{
			if(!shouldStopAttacking(attackMoveData_l, ent_l))
			{
				return false;
			}
		}
		/// clean up attack command
		subAttackCommand_l.cleanUp(step_p, state_p, data_p);
		// get non const pointer here (required by the step)
		step_p.addSteppable(new CommandDelSubCommandStep(_handleCommand, subAttackCommand_l));
		// return false is necessary here to avoid Fixed move step
		return false;
	}
	else if(attackMoveData_l._hasSubBuffCommand)
	{
		if(!subBuffCommand_l.applyCommand(step_p, state_p, data_p, pathManager_p))
		{
			return false;
		}
		/// clean up buff command
		subBuffCommand_l.cleanUp(step_p, state_p, data_p);
		// get non const pointer here (required by the step)
		step_p.addSteppable(new CommandDelSubCommandStep(_handleCommand, subBuffCommand_l));
		// return false is necessary here to avoid Fixed move step
		return false;
	}
	else
	{
		// create command from idle
		Command * cmd_l = commandFromIdle(*ent_l, state_p, 0);
		if(cmd_l)
		{
			CommandVar var_l = getVarFromCommand(cmd_l);
			if(std::holds_alternative<EntityAttackCommand>(var_l))
			{
				EntityAttackCommand const &attackCmd_l = std::get<EntityAttackCommand>(var_l);
				step_p.addSteppable(new CommandSetPositionFromStep(_handleCommand, ent_l->_pos, attackMoveData_l._positionFromAttack));
				/// steppable to update target
				step_p.addSteppable(new CommandNewTargetStep(_handleCommand, attackCmd_l.getTarget(), attackMoveData_l._target));
				step_p.addSteppable(new CommandAddSubCommandStep(_handleCommand, var_l));

				delete cmd_l;
				return false;
			}
			if(std::holds_alternative<EntityBuffCommand>(var_l))
			{
				EntityBuffCommand const &buffCmd_l = std::get<EntityBuffCommand>(var_l);
				step_p.addSteppable(new CommandAddSubCommandStep(_handleCommand, var_l));

				delete cmd_l;
				return false;
			}
		}
		delete cmd_l;
	}

	// run move command
	return _subMoveCommand.applyCommand(step_p, state_p, data_p, pathManager_p);
}

void EntityAttackMoveCommand::cleanUp(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	AttackMoveData const &attackMoveData_l = *static_cast<AttackMoveData const *>(data_p);
	EntityAttackCommand const & subAttackCommand_l = attackMoveData_l._subAttackCommand;
	if(attackMoveData_l._hasSubAttackCommand)
	{
		subAttackCommand_l.cleanUp(step_p, state_p, data_p);
	}
	_subMoveCommand.cleanUp(step_p, state_p, data_p);
}

} // namespace octopus

