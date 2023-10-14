#ifndef __Godoctopus_ModelTypes__
#define __Godoctopus_ModelTypes__

#include <map>
#include <vector>
#include <string>
#include <variant>

#include "utils/Fixed.hh"
#include "utils/Box.hh"

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

	struct GodotTriggerActionDialog { std::string dialog_idx = ""; };
	struct GodotTriggerActionSpawn { std::vector<GodotEntity> entities_to_spawn; };
	using GodotTriggerAction = std::variant<
		GodotTriggerActionDialog,
		GodotTriggerActionSpawn
	>;

	struct GodotTriggerListenerEntityDied { unsigned long entity_group = 0; };
	struct GodotTriggerListenerEntityProduced { unsigned long player = 0; std::string model = ""; };
	struct GodotTriggerListenerTimer { unsigned long steps = 0; };
	struct GodotTriggerListenerResource { unsigned long player = 0; unsigned long quantity = 0; std::string resource = ""; };
	struct GodotTriggerZonePlayer { unsigned long player = 0; std::string zone_name = ""; };
	struct GodotTriggerZoneTeam { unsigned long team = 0; std::string zone_name = ""; };

	// todo add zone
	using GodotTriggerListener = std::variant<
		GodotTriggerListenerEntityDied,
		GodotTriggerListenerTimer,
		GodotTriggerZonePlayer,
		GodotTriggerZoneTeam
	>;

	struct GodotTrigger {
		bool repeatable = false;
		std::vector<GodotTriggerListener> listeners;
		std::vector<GodotTriggerAction> actions;
	};

	struct GodotZone {
		std::string name = "";
		octopus::Box<long> zone;
	};

} // namespace godot

#endif