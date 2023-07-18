#include "CommandHelpers.hh"

#include "command/CommandQueue.hh"
#include "command/building/BuildingRallyPointCommand.hh"
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

Command * newTargetCommand(State const &state_p, Handle const &handle_p,
	Handle const &target_p, Vector const &pos_p, bool targetNotSet_p)
{
	const Entity * cur_l = state_p.getEntity(handle_p);
	const Building * building_l = dynamic_cast<const Building *>(cur_l);
	const Unit * unit_l = dynamic_cast<const Unit *>(cur_l);
	const Player * player_l = state_p.getPlayer(cur_l->_player);

	bool isStatic_l = cur_l->_model._isStatic;

	if(isStatic_l)
	{
		if(building_l)
		{
			// If target is building itself we reset the rally point
			if(!targetNotSet_p && target_p == handle_p)
			{
				return new octopus::BuildingRallyPointCommand(handle_p);
			}
			// Else set the rally point
			else
			{
				return new octopus::BuildingRallyPointCommand(handle_p, pos_p, !targetNotSet_p, target_p);
			}
		}
		return nullptr;
	}

	// if target is not valide anymore
	if(!state_p.isEntityAlive(target_p))
	{
		EntityMoveCommand * command_l = new EntityMoveCommand(
			handle_p,
			handle_p,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}

	const Entity * target_l = state_p.getEntity(target_p);
	const Building * targetBuilding_l = dynamic_cast<const Building *>(target_l);
	const Resource * targetRehandle_p = dynamic_cast<const Resource *>(target_l);
	const Player * targetPlayer_l = state_p.getPlayer(target_l->_player);

	bool alive_l = target_l->_alive || (targetBuilding_l && targetBuilding_l->isBlueprint());
	static FlockInformation flockInfo_l;
	// if target died or if it is not set
	if(!alive_l || targetNotSet_p)
	{
		EntityMoveCommand * command_l = new EntityMoveCommand(
			handle_p,
			handle_p,
			pos_p,
			0,
			{pos_p},
			true
		);
		command_l->setFlockInformation(&flockInfo_l);
		return command_l;
	}
	else if(targetRehandle_p
	&& unit_l
	&& unit_l->_unitModel._maxQuantity.find(targetRehandle_p->_type) != unit_l->_unitModel._maxQuantity.end()
	&& unit_l->_unitModel._maxQuantity.at(targetRehandle_p->_type) > 0)
	{
		UnitHarvestCommand * command_l = new UnitHarvestCommand(
			handle_p,
			handle_p,
			target_p,
			pos_p,
			0,
			{pos_p},
			true
		);
		return command_l;
	}
	else if(target_l && !targetRehandle_p
	&& player_l->_team != targetPlayer_l->_team)
	{
		EntityAttackCommand * command_l = new EntityAttackCommand(
			handle_p,
			handle_p,
			target_p,
			true
		);
		return command_l;
	}
	else if(targetBuilding_l
	&& !targetBuilding_l->isBuilt())
	{
		EntityBuildingCommand * command_l = new EntityBuildingCommand(
			handle_p,
			handle_p,
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
			handle_p,
			handle_p,
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
			handle_p,
			handle_p,
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
