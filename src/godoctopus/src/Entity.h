#ifndef __Godoctopus_Entity__
#define __Godoctopus_Entity__

#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"

namespace godot {

class Controller;

class EntityHandle : public Node {
    GDCLASS(EntityHandle, Node)

public:
    void set_index(int index_p) { index = index_p; }
    int get_index() const { return index; }

    void set_revision(int revision_p) { revision = revision_p; }
    int get_revision() const { return revision; }

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();
private:
    int index = 0;
    int revision = 0;
};

octopus::Handle castHandle(godot::EntityHandle const *handle_p);
octopus::Handle castHandle(godot::Variant const &var_p);

class Buff : public Node {
    GDCLASS(Buff, Node)

public:
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
    Entity();
    ~Entity();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    // property setter
    void set_index(int index_p);
    void set_revision(int revision_p);

    // property getter
    EntityHandle * get_handle() const;
    int get_index() const;
    int get_revision() const;
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
    int _index = 0;
    int _revision = 0;
    EntityHandle * _handle = nullptr;
};

}

#endif //__Godoctopus_Entity__
