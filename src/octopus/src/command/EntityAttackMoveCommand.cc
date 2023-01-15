
#include "EntityAttackMoveCommand.hh"

#include <cmath>
#include "EntityAttackCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandSetPositionFromStep.hh"
#include "step/command/CommandSubAttackStep.hh"
#include "step/entity/EntityAttackStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityAttackMoveCommand::EntityAttackMoveCommand(Handle const &commandHandle_p, Handle const &source_p, std::list<Vector> const &waypoints_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _waypoints(waypoints_p)
	, _subMoveCommand(commandHandle_p, source_p, waypoints_p)
{}

bool shouldStopAttacking(AttackMoveData const &attackMoveData_p, Entity const *ent_p)
{
	/// distance when to stop running after target (squared)
	double maxSqDistance_l = 25.;
	Vector diff_l = ent_p->_pos - attackMoveData_p._positionFromAttack;
	return square_length(diff_l) > maxSqDistance_l;
}

bool EntityAttackMoveCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	Logger::getDebug() << "EntityAttackMoveCommand:: apply Command "<<_source <<std::endl;
	AttackMoveData const &attackMoveData_l = static_cast<CommandDataWithData<AttackMoveData> const *>(data_p)->_data;
	EntityAttackCommand const * subAttackCommand_l = attackMoveData_l._subAttackCommand;
	Entity const * ent_l = state_p.getEntity(_source);

	if(subAttackCommand_l)
	{
		if(!subAttackCommand_l->applyCommand(step_p, state_p, data_p))
		{
			if(!shouldStopAttacking(attackMoveData_l, ent_l))
			{
				return false;
			}
		}
		// get non const pointer here (required by the step)
		step_p.addSteppable(new CommandDelSubAttackStep(_handleCommand, attackMoveData_l._subAttackCommand));
	}
	else
	{
		Entity const * target_l = lookUpNewTarget(state_p, _source);
		if(target_l)
		{
			step_p.addSteppable(new CommandSetPositionFromStep(_handleCommand, ent_l->_pos, attackMoveData_l._positionFromAttack));
			step_p.addSteppable(new CommandAddSubAttackStep(_handleCommand, new EntityAttackCommand(_handleCommand, _source, target_l->_handle)));
			return false;
		}
	}

	// run move command
	return _subMoveCommand.applyCommand(step_p, state_p, data_p);
}

} // namespace octopus

