#include "CommandHelpers.hh"

#include "command/CommandQueue.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace octopus
{

Command * newTargetCommand(State const &state_p, Handle const &source_l,
	Handle const &target_p, Vector const &pos_p, bool targetNotSet_p)
{
	const Entity * cur_l = state_p.getEntity(source_l);
	const Unit * unit_l = dynamic_cast<const Unit *>(cur_l);
	const Player * player_l = state_p.getPlayer(cur_l->_player);

	bool isStatic_l = cur_l->_model._isStatic;

	if(isStatic_l)
	{
		return nullptr;
	}

	const Entity * target_l = state_p.getEntity(target_p);
	const Building * targetBuilding_l = dynamic_cast<const Building *>(target_l);
	const Resource * targetResource_l = dynamic_cast<const Resource *>(target_l);
	const Player * targetPlayer_l = state_p.getPlayer(target_l->_player);

	// if target died or if it is not set
	if(!target_l->_alive || targetNotSet_p)
	{
		EntityMoveCommand * command_l = new EntityMoveCommand(
			source_l,
			source_l,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}
	else if(targetResource_l
	&& unit_l
	&& unit_l->_unitModel._maxQuantity.find(targetResource_l->_type) != unit_l->_unitModel._maxQuantity.end()
	&& unit_l->_unitModel._maxQuantity.at(targetResource_l->_type) > 0)
	{
		UnitHarvestCommand * command_l = new UnitHarvestCommand(
			source_l,
			source_l,
			target_p,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}
	else if(target_l && !targetResource_l
	&& player_l->_team != targetPlayer_l->_team)
	{
		EntityAttackCommand * command_l = new EntityAttackCommand(
			source_l,
			source_l,
			target_p,
			true
		);
		return command_l;
	}
	else if(targetBuilding_l
	&& !targetBuilding_l->isBuilt())
	{
		EntityBuildingCommand * command_l = new EntityBuildingCommand(
			source_l,
			source_l,
			target_p,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}
	else if(unit_l && targetBuilding_l
	&& targetBuilding_l->_buildingModel.isDeposit(unit_l->_typeOfResource)
	&& unit_l->_quantityOfResource > 0)
	{
		UnitDropCommand * command_l = new UnitDropCommand(
			source_l,
			source_l,
			target_p,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}
	else
	{
		EntityMoveCommand * command_l = new EntityMoveCommand(
			source_l,
			source_l,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}
	return nullptr;
}

} // namespace octopus
