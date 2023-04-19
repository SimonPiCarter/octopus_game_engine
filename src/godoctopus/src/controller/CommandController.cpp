#include "CommandController.h"

// octopus
#include "command/CommandQueue.hh"
#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace godot
{

void add_move_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        const octopus::Entity * cur_l = state_p.getEntity(idx_l);
		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }
        controller_p.commitCommandAsPlayer(new octopus::EntityMoveCommand(idx_l, idx_l, octopus::Vector(target_p.x, target_p.y), 0, {octopus::Vector(target_p.x, target_p.y)}, true), player_p);
    }
}

void add_move_target_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        octopus::Vector worldPos_l(target_p.x, target_p.y);

        const octopus::Entity * cur_l = state_p.getEntity(idx_l);
		const octopus::Unit * unit_l = dynamic_cast<const octopus::Unit *>(cur_l);
		const octopus::Player * player_l = state_p.getPlayer(cur_l->_player);

		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }

        const octopus::Entity * target_l = state_p.getEntity(handleTarget_p);
        const octopus::Building * targetBuilding_l = dynamic_cast<const octopus::Building *>(target_l);
        const octopus::Resource * targetResource_l = dynamic_cast<const octopus::Resource *>(target_l);
        const octopus::Player * targetPlayer_l = state_p.getPlayer(target_l->_player);

        if(targetResource_l
        && unit_l
        && unit_l->_unitModel._maxQuantity.find(targetResource_l->_type) != unit_l->_unitModel._maxQuantity.end())
        {
            octopus::UnitHarvestCommand * command_l = new octopus::UnitHarvestCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            controller_p.commitCommandAsPlayer(command_l, player_p);
        }
        else if(target_l && !targetResource_l
        && player_l->_team != targetPlayer_l->_team)
        {
            octopus::EntityAttackCommand * command_l = new octopus::EntityAttackCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                true
            );
            controller_p.commitCommandAsPlayer(command_l, player_p);
        }
        else if(targetBuilding_l
        && !targetBuilding_l->isBuilt())
        {
            octopus::EntityBuildingCommand * command_l = new octopus::EntityBuildingCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            controller_p.commitCommandAsPlayer(command_l, player_p);
        }
        else if(unit_l && targetBuilding_l
        && targetBuilding_l->_buildingModel.isDeposit(unit_l->_typeOfResource)
        && unit_l->_quantityOfResource > 0)
        {
            octopus::UnitDropCommand * command_l = new octopus::UnitDropCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            controller_p.commitCommandAsPlayer(command_l, player_p);
        }
        else
        {
            octopus::EntityMoveCommand * command_l = new octopus::EntityMoveCommand(
                idx_l,
                idx_l,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            controller_p.commitCommandAsPlayer(command_l, player_p);
        }
    }
}

void add_attack_move_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        const octopus::Entity * cur_l = state_p.getEntity(idx_l);
		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }
        controller_p.commitCommandAsPlayer(new octopus::EntityAttackMoveCommand(idx_l, idx_l, octopus::Vector(target_p.x, target_p.y), 0, {octopus::Vector(target_p.x, target_p.y)}, true), player_p);
    }
}

unsigned long remainingQueueTime(octopus::Building const &building_p)
{
	// remaining queue time
	unsigned long time_l = 0;

	if(building_p.getQueue().hasCommand())
	{
		auto it_l = building_p.getQueue().getCurrentCommand();
		while(it_l != building_p.getQueue().getEnd())
		{
			octopus::BuildingUnitProductionCommand const *cmd_l = dynamic_cast<octopus::BuildingUnitProductionCommand const *>(it_l->_cmd);
			octopus::UnitProductionData const *data_l = dynamic_cast<octopus::UnitProductionData const *>(it_l->_data);
			if(cmd_l && data_l
			&& data_l->_completeTime > data_l->_progression)
			{
				time_l += data_l->_completeTime - data_l->_progression;
			}
			++it_l;
		}
	}

	return time_l;
}

int getBestProductionBuilding(TypedArray<int> const &handles_p, octopus::State const &state_p, octopus::UnitModel const &model_p)
{
    int best_l = -1;
    unsigned long lowestQueue_l = 0;
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        // check ent
        octopus::Entity const *ent_l = state_p.getEntity(idx_l);
        // skip non building
        if(!ent_l->_model._isBuilding)
        {
            continue;
        }
        octopus::Building const *building_l = dynamic_cast<octopus::Building const *>(ent_l);

        // skip if cannot produce building
        if(!building_l->_buildingModel.canProduce(&model_p)
        || !building_l->isBuilt())
        {
            continue;
        }

        // get production time queued up
        unsigned long queueTime_l = remainingQueueTime(*building_l);

        if(best_l < 0 || queueTime_l < lowestQueue_l)
        {
            best_l = idx_l;
            lowestQueue_l = queueTime_l;
        }
    }

    return best_l;
}

void add_unit_build_command(octopus::Controller &controller_p, octopus::State const &state_p, octopus::Library const &lib_p, TypedArray<int> const &handles_p, String const &model_p, int player_p)
{
    std::string modelId_l(model_p.utf8().get_data());

    if(lib_p.hasUnitModel(modelId_l))
    {
        octopus::UnitModel const &unit_l = lib_p.getUnitModel(modelId_l);
        int best_l = getBestProductionBuilding(handles_p, state_p, unit_l);

        if(best_l >= 0)
        {
            octopus::BuildingUnitProductionCommand *cmd_l = new octopus::BuildingUnitProductionCommand(best_l, best_l, unit_l);
            cmd_l->setQueued(true);
            controller_p.commitCommandAsPlayer(cmd_l, player_p);
        }
    }
}

void add_unit_build_cancel_command(octopus::Controller &controller_p, octopus::State const &state_p, int handle_p, int index_p, int player_p)
{
    controller_p.commitCommandAsPlayer(new octopus::BuildingUnitCancelCommand(handle_p, index_p), player_p);
}

void add_blueprint_command(octopus::Controller &controller_p, octopus::State const &state_p, octopus::Library const &lib_p, Vector2 const &target_p, String const &model_p, int player_p)
{
    std::string modelId_l(model_p.utf8().get_data());

    if(lib_p.hasBuildingModel(modelId_l))
    {
        controller_p.commitCommandAsPlayer(new octopus::BuildingBlueprintCommand(octopus::Vector(target_p.x, target_p.y), player_p, lib_p.getBuildingModel(modelId_l)), player_p);
    }
}

} // godot
