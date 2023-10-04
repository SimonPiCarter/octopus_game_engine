#ifndef __Godoctopus_ModelTypes__
#define __Godoctopus_ModelTypes__

#include <map>
#include <vector>
#include <string>

#include "utils/Fixed.hh"

namespace godot
{

    /// @brief stored data about players
    struct GodotPlayer {
        std::map<std::string, octopus::Fixed> resources;
        std::vector<std::string> buildings;
        unsigned long team;
    };

    struct GodotEntity {
        std::string type;
        std::string model;
        unsigned long player;
        float x;
        float y;
		/// @brief list of entity group of this entity
		std::vector<unsigned long> entity_group;
		/// @brief number of player required to spawn the entity
		unsigned long num_players_to_spawn = 0;
    };

	struct GodotTriggerAction {
		bool dialog_enabled = false;
		std::string dialog_idx = "";

		bool unit_spawn = false;
		std::vector<GodotEntity> entities_to_spawn;
	};

	struct GodotTrigger {
		// trigger if a group en entity is dead
		bool entity_dead_trigger = false;
		unsigned long entity_dead_group = 0;

		// todo add zone

		// action to be done when triggering
		GodotTriggerAction action;
	};

} // namespace godot

#endif