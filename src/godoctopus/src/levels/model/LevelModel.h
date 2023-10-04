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
	void set_trigger_entity_dead_group(int triggerIdx_p, int entityDeadGroup_p);
	void set_trigger_action_dialog(int triggerIdx_p, String const &dialogIdx_p);
	void add_trigger_action_spawn_entity(int triggerIdx_p, String const &type_p, String const &model_p, int player, float x, float y, int num_of_players);

    ////////////////
    /// generator (steps)
    ////////////////
    std::list<octopus::Steppable *> generateLevelSteps(octopus::Library const &lib_p, unsigned long playerCount_p);

private:
    /// @brief players to spawn
    std::vector<GodotPlayer> _players;

    std::vector<GodotEntity> _entities;

    std::vector<GodotTrigger> _triggers;
};

}

#endif //__Godoctopus_LevelModel__
