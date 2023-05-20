#ifndef __Godoctopus_LevelModel__
#define __Godoctopus_LevelModel__

#include <string>
#include <cstdint>
#include <map>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"
#include "utils/Fixed.hh"

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
    /// player
    ////////////////
    void add_entity(String const &type_p, String const &model_p, int player, float x, float y);


    ////////////////
    /// generator (steps)
    ////////////////
    std::list<octopus::Steppable *> generateLevelSteps(octopus::Library const &lib_p);

private:
    /// @brief stored data about players
    struct GodotPlayer {
        std::map<std::string, octopus::Fixed> resources;
        std::vector<std::string> buildings;
        unsigned long team;
    };

    /// @brief players to spawn
    std::vector<GodotPlayer> _players;

    struct GodotEntity {
        std::string type;
        std::string model;
        unsigned long player;
        float x;
        float y;
    };

    std::vector<GodotEntity> _entities;
};

}

#endif //__Godoctopus_LevelModel__
