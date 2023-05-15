#include "CommandController.h"

// octopus
#include "command/CommandHelpers.hh"
#include "command/CommandQueue.hh"
#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
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

void add_move_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        octopus::Vector worldPos_l(target_p.x, target_p.y);
        int handle_l = handles_p[i];
        octopus::Command *cmd_l = octopus::newTargetCommand(state_p, handle_l, 0, worldPos_l, true);
        if(cmd_l)
        {
            cmd_l->setQueued(queued_p);
            controller_p.queueCommandAsPlayer(cmd_l, player_p);
        }
    }
}

void add_move_target_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p, bool queued_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        octopus::Vector worldPos_l(target_p.x, target_p.y);
        int handle_l = handles_p[i];
        octopus::Command *cmd_l = octopus::newTargetCommand(state_p, handle_l, handleTarget_p, worldPos_l, false);
        if(cmd_l)
        {
            cmd_l->setQueued(queued_p);
            controller_p.queueCommandAsPlayer(cmd_l, player_p);
        }
    }
}

void add_attack_move_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p)
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
        octopus::Command *cmd_l = new octopus::EntityAttackMoveCommand(idx_l, idx_l, octopus::Vector(target_p.x, target_p.y), 0, {octopus::Vector(target_p.x, target_p.y)}, true);
        cmd_l->setQueued(queued_p);
        controller_p.queueCommandAsPlayer(cmd_l, player_p);
    }
}

void add_stop_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, int player_p, bool queued_p)
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
        octopus::Command *cmd_l = new octopus::EntityWaitCommand(idx_l, idx_l);
        cmd_l->setQueued(queued_p);
        controller_p.queueCommandAsPlayer(cmd_l, player_p);
    }
}

unsigned long remainingQueueTime(octopus::Building const &building_p)
{
	// remaining queue time
	unsigned long time_l = 0;

	if(building_p.getQueue().hasCommand())
	{
	    for(octopus::CommandBundle const &bundle_l : building_p.getQueue().getList())
		{
			octopus::UnitProductionData const *data_l = dynamic_cast<octopus::UnitProductionData const *>(getData(bundle_l._var));
			if(data_l && data_l->_completeTime > data_l->_progression)
			{
				time_l += data_l->_completeTime - data_l->_progression;
			}
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
            controller_p.queueCommandAsPlayer(cmd_l, player_p);
        }
    }
}

void add_unit_build_cancel_command(octopus::Controller &controller_p, octopus::State const &state_p, int handle_p, int index_p, int player_p)
{
    controller_p.queueCommandAsPlayer(new octopus::BuildingUnitCancelCommand(handle_p, index_p), player_p);
}

void add_blueprint_command(octopus::Controller &controller_p, octopus::State const &state_p, octopus::Library const &lib_p, Vector2 const &target_p, String const &model_p, int player_p, TypedArray<int> const &builders_p)
{
    std::string modelId_l(model_p.utf8().get_data());
    std::vector<octopus::Handle> builders_l;
    for(size_t i = 0 ; i < builders_p.size() ; ++ i)
    {
        int idx_l = builders_p[i];
        builders_l.push_back(idx_l);
    }

    if(lib_p.hasBuildingModel(modelId_l))
    {
        controller_p.queueCommandAsPlayer(new octopus::BuildingBlueprintCommand(octopus::Vector(target_p.x, target_p.y), player_p, lib_p.getBuildingModel(modelId_l), builders_l), player_p);
    }
}

} // godot
