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
		unsigned long resource_qty = 2000;
    };

	struct GodotTriggerActionDialog { std::string dialog_idx = ""; bool end=false; int winning_team=0; };
	struct GodotTriggerActionSpawn { std::vector<GodotEntity> entities_to_spawn; bool attack_move=false; float x=0; float y=0; };
	struct GodotTriggerActionCamera { int x=0; int y=0; int player=0; };
	struct GodotTriggerActionZoneDamage { int damage = 0; unsigned long team = 0; std::string zone_name = ""; octopus::Box<long> zone; };
	struct GodotTriggerActionAddObjective { std::string obj_name = ""; bool is_main=true; int count=-1; bool remove=false; };
	struct GodotTriggerActionCompleteObjective { std::string obj_name = ""; bool complete=true; };
	struct GodotTriggerActionFailObjective { std::string obj_name = ""; bool fail=true; };
	struct GodotTriggerActionIncrementObjective { std::string obj_name = ""; bool increment=true; };
	struct GodotTriggerActionResource { unsigned long player=0; int qty = 0; std::string resource = "";};
	using GodotTriggerAction = std::variant<
		GodotTriggerActionDialog,
		GodotTriggerActionSpawn,
		GodotTriggerActionCamera,
		GodotTriggerActionZoneDamage,
		GodotTriggerActionAddObjective,
		GodotTriggerActionCompleteObjective,
		GodotTriggerActionFailObjective,
		GodotTriggerActionIncrementObjective,
		GodotTriggerActionResource
	>;

	struct GodotTriggerListenerEntityDied { unsigned long entity_group = 0; };
	struct GodotTriggerListenerEntityProduced { unsigned long player = 0; std::string model = ""; };
	struct GodotTriggerListenerTimer { unsigned long steps = 0; };
	struct GodotTriggerZonePlayer { unsigned long player = 0; std::string zone_name = ""; };
	struct GodotTriggerZoneTeam { unsigned long team = 0; std::string zone_name = ""; };
	struct GodotTriggerListenerResource { unsigned long player = 0; std::string resource = ""; int qty=0; bool lower_than=false; };
	struct GodotTriggerEntityProduced { unsigned long player = 0; std::string model = ""; };

	// todo add zone
	using GodotTriggerListener = std::variant<
		GodotTriggerListenerEntityDied,
		GodotTriggerListenerTimer,
		GodotTriggerZonePlayer,
		GodotTriggerZoneTeam,
		GodotTriggerListenerResource,
		GodotTriggerEntityProduced
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