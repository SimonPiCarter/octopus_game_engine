#include "EntitySpawner.h"

#include "library/Library.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"

#include "ModelTypes.h"

namespace godot {

	/// @brief compute entity handle info of the level
	std::vector<GodotEntityInfo> getEntityInfo(std::vector<GodotEntity> const &entities_p, unsigned long playerCount_p, unsigned long startHandle_p)
	{
		std::vector<GodotEntityInfo> info_l;
		unsigned long handle_l = startHandle_p;
		for(unsigned long idx_l = 0 ; idx_l < entities_p.size() ; ++idx_l)
		{
			GodotEntity const &ent_l = entities_p.at(idx_l);
			if(ent_l.num_players_to_spawn <= playerCount_p)
			{
				info_l.push_back({ent_l.model, handle_l, ent_l.player, (unsigned long)ent_l.x, (unsigned long)ent_l.y});
				++handle_l;
			}
		}
		return info_l;
	}

	std::vector<unsigned long> getHandles(std::vector<GodotEntityInfo> const &info_p, unsigned long player_p, std::string const &model_p)
	{
		std::vector<unsigned long> handles_l;

		for(GodotEntityInfo const &info_l : info_p)
		{
			if(info_l.model == model_p && info_l.player == player_p)
			{
				handles_l.push_back(info_l.handle);
			}
		}

		return handles_l;
	}

	bool spawnEntity(std::list<octopus::Steppable *> &steps_r, octopus::Handle const&idx_p, GodotEntity const &ent_p,
		octopus::Library const &lib_p, unsigned long playerCount_p)
	{
		if(ent_p.num_players_to_spawn > playerCount_p)
		{
			return false;
		}
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
		return true;
	}

}
