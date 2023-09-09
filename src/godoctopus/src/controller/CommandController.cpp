#include "CommandController.h"

#include <limits>

// octopus
#include "command/CommandHelpers.hh"
#include "command/CommandQueue.hh"
#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityFlockMoveCommand.hh"
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

void add_move_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, Vector2 const &target_p, int , bool queued_p)
{
    bool isFlocking_l = false;
    octopus::Vector worldPos_l(target_p.x, target_p.y);
    for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        octopus::Command *cmd_l = octopus::newTargetCommand(state_p, castHandle(handles_p[i*2],handles_p[i*2+1]), octopus::Handle(0), worldPos_l, true);
        if(dynamic_cast<octopus::EntityMoveCommand*>(cmd_l))
        {
            isFlocking_l = true;
            delete cmd_l;
            break;
        }
        else if(cmd_l)
        {
            cmd_l->setQueued(queued_p);
            list_r.push_back(cmd_l);
        }
    }
    if(isFlocking_l)
    {
        std::list<octopus::Handle> handles_l;
        for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
        {
            handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
        }
        octopus::Command * cmd_l = new octopus::EntityFlockMoveCommand(handles_l, worldPos_l, false);

        cmd_l->setQueued(queued_p);
        list_r.push_back(cmd_l);
    }
}

void add_move_target_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, Vector2 const &target_p, PackedInt32Array const & handleTarget_p, int , bool queued_p)
{
	std::list<octopus::Handle> flock_l;
	octopus::Vector worldPos_l(target_p.x, target_p.y);
	for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
	{
		octopus::Handle handle_l = castHandle(handles_p[i*2],handles_p[i*2+1]);
		octopus::Command *cmd_l = octopus::newTargetCommand(state_p, handle_l, castHandle(handleTarget_p[0], handleTarget_p[1]), worldPos_l, false);
		if(dynamic_cast<octopus::EntityMoveCommand*>(cmd_l))
		{
			delete cmd_l;
			flock_l.push_back(handle_l);
		}
		else if(cmd_l)
		{
			cmd_l->setQueued(queued_p);
			list_r.push_back(cmd_l);
		}
	}

	if(!flock_l.empty())
	{
		octopus::Command * cmd_l = new octopus::EntityFlockMoveCommand(flock_l, worldPos_l, false);

		cmd_l->setQueued(queued_p);
		list_r.push_back(cmd_l);
	}
}

void add_attack_move_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, Vector2 const &target_p, int, bool queued_p)
{
    octopus::Vector worldPos_l(target_p.x, target_p.y);
    std::list<octopus::Handle> handles_l;
    for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }
    octopus::Command * cmd_l = new octopus::EntityFlockMoveCommand(handles_l, worldPos_l, true);

    cmd_l->setQueued(queued_p);
    list_r.push_back(cmd_l);
}

void add_stop_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, int, bool queued_p)
{
    for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        octopus::Handle idx_l = castHandle(handles_p[i*2],handles_p[i*2+1]);
        const octopus::Entity * cur_l = state_p.getEntity(idx_l);
		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }
        octopus::Command *cmd_l = new octopus::EntityWaitCommand(idx_l, idx_l);
        cmd_l->setQueued(queued_p);
        list_r.push_back(cmd_l);
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
			octopus::ProductionData const *data_l = dynamic_cast<octopus::ProductionData const *>(getData(bundle_l._var));
			if(data_l && data_l->_completeTime > data_l->_progression)
			{
				time_l += octopus::to_double(data_l->_completeTime - data_l->_progression);
			}
		}
	}

	return time_l;
}

template<typename production_t>
octopus::Handle getBestProductionBuilding(PackedInt32Array const &handles_p, octopus::State const &state_p, production_t const &model_p)
{
    bool found_l = false;
    octopus::Handle best_l {std::numeric_limits<unsigned long>::max(), 0};
    unsigned long lowestQueue_l = 0;
    for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        octopus::Handle idx_l = castHandle(handles_p[i*2],handles_p[i*2+1]);
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

        if(!found_l || queueTime_l < lowestQueue_l)
        {
            found_l = true;
            best_l = idx_l;
            lowestQueue_l = queueTime_l;
        }
    }

    return best_l;
}

void add_unit_build_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, octopus::Library const &lib_p, PackedInt32Array const &handles_p, String const &model_p, int)
{
    std::string modelId_l(model_p.utf8().get_data());

    if(lib_p.hasUnitModel(modelId_l))
    {
        octopus::UnitModel const &unit_l = lib_p.getUnitModel(modelId_l);
        octopus::Handle best_l = getBestProductionBuilding(handles_p, state_p, unit_l);

        if(best_l.index < state_p.getEntities().size())
        {
            octopus::BuildingUnitProductionCommand *cmd_l = new octopus::BuildingUnitProductionCommand(best_l, best_l, unit_l);
            cmd_l->setQueued(true);
            list_r.push_back(cmd_l);
        }
    }
    else if(lib_p.hasUpgrade(modelId_l))
    {
        octopus::Upgrade const &upgrade_l = lib_p.getUpgrade(modelId_l);
        octopus::Handle best_l = getBestProductionBuilding(handles_p, state_p, upgrade_l);

        if(best_l.index < state_p.getEntities().size())
        {
            octopus::BuildingUpgradeProductionCommand *cmd_l = new octopus::BuildingUpgradeProductionCommand(best_l, best_l, upgrade_l);
            cmd_l->setQueued(true);
            list_r.push_back(cmd_l);
        }
    }
}

void add_unit_build_cancel_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handle_p, int index_p, int)
{
    list_r.push_back(new octopus::BuildingUnitCancelCommand(castHandle(handle_p[0], handle_p[1]), index_p));
}

void add_blueprint_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, octopus::Library const &lib_p, Vector2 const &target_p,
    String const &model_p, int player_p, PackedInt32Array const &builders_p, bool queued_p)
{
    std::string modelId_l(model_p.utf8().get_data());
    std::vector<octopus::Handle> builders_l;
    for(size_t i = 0 ; i < builders_p.size()/2 ; ++ i)
    {
        builders_l.push_back(castHandle(builders_p[2*i], builders_p[2*i+1]));
    }

    if(lib_p.hasBuildingModel(modelId_l))
    {
        octopus::Command * cmd_l = new octopus::BuildingBlueprintCommand(octopus::Vector(target_p.x, target_p.y), player_p, lib_p.getBuildingModel(modelId_l), builders_l);
        cmd_l->setQueued(queued_p);
        list_r.push_back(cmd_l);
    }
}

void add_ability_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, String const &ability_p)
{
    std::string abilityId_l(ability_p.utf8().get_data());
    std::list<octopus::Handle> handles_l;
    for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }

	for(octopus::Handle const &handle_l : handles_l)
	{
        octopus::Command * cmd_l = new octopus::EntityAbilityCommand(handle_l, handle_l, octopus::Vector(), abilityId_l);
        list_r.push_back(cmd_l);
	}
}

} // godot
