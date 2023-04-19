#ifndef __Godoctopus_Entity__
#define __Godoctopus_Entity__

#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"

namespace godot {

class Controller;

class Entity : public Node {
    GDCLASS(Entity, Node)

public:
    ~Entity();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    // property setter
    void set_handle(int handle_p);

    // property getter
    int get_handle() const;
    float get_hp(Controller const *controller_p) const;
    float get_hp_max(Controller const *controller_p) const;
    Vector2 get_pos(Controller const *controller_p) const;
    bool is_alive(Controller const *controller_p) const;
    bool is_blueprint(Controller const *controller_p) const;
    bool is_building(Controller const *controller_p) const;
    bool is_resource(Controller const *controller_p) const;
    bool is_unit(Controller const *controller_p) const;
    int get_player(Controller const *controller_p) const;
    int get_team(Controller const *controller_p) const;
    String get_model(Controller const *controller_p) const;
    float get_damage(Controller const *controller_p) const;
    float get_armor(Controller const *controller_p) const;
    float get_speed(Controller const *controller_p) const;
    float get_reload(Controller const *controller_p) const;

private:
    int _handle;
};

}

#endif //__Godoctopus_Entity__
