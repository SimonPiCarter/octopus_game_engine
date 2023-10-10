#include "EntitySpawner.h"

#include "library/Library.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"

#include "ModelTypes.h"

namespace godot {

	void spawnEntity(std::list<octopus::Steppable *> &steps_r, octopus::Handle const&idx_p, GodotEntity const &ent_p,
		octopus::Library const &lib_p, unsigned long playerCount_p)
	{
		/// @todo handle idx_p better in that case
		// if(ent_p.num_players_to_spawn > playerCount_p)
		// {
		// 	return;
		// }
        if(ent_p.type == "Unit")
        {
	        octopus::Unit unit_l({ ent_p.x, ent_p.y }, false, lib_p.getUnitModel(ent_p.model));
            unit_l._player = ent_p.player;
            steps_r.push_back(new octopus::UnitSpawnStep(idx_p, unit_l));
        }
        else if(ent_p.type == "Building")
        {
	        octopus::Building building_l({ ent_p.x, ent_p.y }, true, lib_p.getBuildingModel(ent_p.model));
            building_l._player = ent_p.player;
            steps_r.push_back(new octopus::BuildingSpawnStep(idx_p, building_l, true));
        }
        else if(ent_p.type == "Resource")
        {
	        octopus::Resource resource_l({ ent_p.x, ent_p.y }, true, lib_p.getResourceModel(ent_p.model));
			resource_l._resource = 2000;
            resource_l._player = ent_p.player;
            steps_r.push_back(new octopus::ResourceSpawnStep(idx_p, resource_l));
        }
        else if(ent_p.type == "Anchor")
        {
	        octopus::Building building_l({ ent_p.x, ent_p.y }, true, lib_p.getBuildingModel(ent_p.model));
            building_l._player = ent_p.player;
            steps_r.push_back(new octopus::BuildingSpawnStep(idx_p, building_l, true));
            steps_r.push_back(new octopus::StateTemplePositionAddStep({ ent_p.x, ent_p.y }));
        }
        else
        {
            throw std::logic_error("Unkown type found in entities when generating level model");
        }
	}

}
