#ifndef __Godoctopus_LevelModel__
#define __Godoctopus_LevelModel__

#include <string>
#include <cstdint>
#include <map>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"
#include "utils/Fixed.hh"
#include "utils/ModelTypes.h"

namespace octopus
{
    class Library;
    class Steppable;
} // namespace octopus


namespace godot {

class Controller;

class LevelModel : public Node {
    GDCLASS(LevelModel, Node)

public:
    ~LevelModel();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    ////////////////
    /// player
    ////////////////
    int add_player(int team);
    void add_resource(int player, String const &resource, float quantity);
    void add_building(int player, String const &building);

    ////////////////
    /// entities
    ////////////////
    void add_entity(String const &type_p, String const &model_p, int player, float x, float y, PackedInt32Array const &array_p, int num_of_players);

    ////////////////
    /// triggers
    ////////////////
	int add_trigger();
	void add_trigger_listener_entity_dead_group(int triggerIdx_p, int entityDeadGroup_p);
	void add_trigger_listener_timer(int triggerIdx_p, int steps_p);
	void add_trigger_listener_zone_player(int triggerIdx_p, int player_p, String const &zone_p);
	void add_trigger_listener_zone_team(int triggerIdx_p, int team_p, String const &zone_p);
	void add_trigger_action_dialog(int triggerIdx_p, String const &dialogIdx_p, bool end_p, int team_winning_p);
	void add_trigger_action_spawn_entity(int triggerIdx_p, String const &type_p, String const &model_p, int player, float x, float y, int num_of_players);
	void add_trigger_action_camera(int triggerIdx_p, int x, int y, int player_p);

    ////////////////
    /// zones
    ////////////////
	void add_zone(String const &name_p, int x, int y, int size_x, int size_y);

    ////////////////
    /// generator (steps)
    ////////////////
    std::list<octopus::Steppable *> generateLevelSteps(octopus::Library const &lib_p, unsigned long playerCount_p);

private:
    /// @brief players to spawn
    std::vector<GodotPlayer> _players;

    std::vector<GodotEntity> _entities;

    std::vector<GodotTrigger> _triggers;

	std::vector<GodotZone> _zones;
};

}

#endif //__Godoctopus_LevelModel__
