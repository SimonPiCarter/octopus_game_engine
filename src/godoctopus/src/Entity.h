#ifndef __Godoctopus_Entity__
#define __Godoctopus_Entity__

#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"

namespace godot {

class Controller;

class Buff : public Node {
    GDCLASS(Buff, Node)

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    void setTimeElapsed(float const &timeElapsed_p) { _timeElapsed = timeElapsed_p; }
    float getTimeElapsed() const { return _timeElapsed; }

    void setDuration(float const &duration_p) { _duration = duration_p; }
    float getDuration() const { return _duration; }

    void setName(String const &name_p) { _name = name_p; }
    String getName() const { return _name; }

private:
    float _timeElapsed = 0;
    float _duration = 0;
    String _name;
};

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
    bool is_built(Controller const *controller_p) const;
    bool is_resource(Controller const *controller_p) const;
    bool is_unit(Controller const *controller_p) const;
    bool is_idle(Controller const *controller_p) const;
    int get_player(Controller const *controller_p) const;
    int get_team(Controller const *controller_p) const;
    float get_ray(Controller const *controller_p) const;
    String get_model(Controller const *controller_p) const;
    float get_damage(Controller const *controller_p) const;
    float get_armor(Controller const *controller_p) const;
    float get_speed(Controller const *controller_p) const;
    float get_reload(Controller const *controller_p) const;

    // building getter
    bool has_rally_point(Controller const *controller_p) const;
    Vector2 get_rally_point(Controller const *controller_p) const;

    // resource getter
    String get_resource_type(Controller const *controller_p) const;
    float get_resource_quantity(Controller const *controller_p) const;

    // buff getter
    TypedArray<godot::Buff> get_buffs(Controller const *controller_p) const;

private:
    int _handle;
};

}

#endif //__Godoctopus_Entity__
