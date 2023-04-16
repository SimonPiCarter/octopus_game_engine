#include "Entity.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include "library/levels/MazeLevel.hh"
#include "library/levels/ArenaLevel.hh"
#include "library/levels/WaveLevel.hh"

// octopus
#include "state/entity/Entity.hh"

namespace godot {

Entity::~Entity()
{}

void Entity::set_handle(int handle_p) {
    _handle = handle_p;
}

int Entity::get_handle() const
{
    return _handle;
}

float Entity::get_hp(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return octopus::to_double(ent_l->_hp);
}

float Entity::get_hp_max(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return octopus::to_double(ent_l->getHpMax());
}

Vector2 Entity::get_pos(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return Vector2(octopus::to_double(ent_l->_pos.x), octopus::to_double(ent_l->_pos.y));
}

bool Entity::is_alive(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_alive;
}

int Entity::get_player(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_player;
}

String Entity::get_model(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_model._id.c_str();
}

float Entity::get_damage(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->getDamageNoBonus();
}

float Entity::get_armor(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->getArmor();
}

float Entity::get_speed(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->getStepSpeed()*100;
}

float Entity::get_reload(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return 100./ent_l->getFullReload();
}

void Entity::_bind_methods()
{
    UtilityFunctions::print("Binding Entity methods");

    ClassDB::bind_method(D_METHOD("set_handle", "handle"), &Entity::set_handle);
    ClassDB::bind_method(D_METHOD("get_handle"), &Entity::get_handle);
    ClassDB::bind_method(D_METHOD("get_hp", "controller"), &Entity::get_hp);
    ClassDB::bind_method(D_METHOD("get_hp_max", "controller"), &Entity::get_hp_max);
    ClassDB::bind_method(D_METHOD("get_pos", "controller"), &Entity::get_pos);
    ClassDB::bind_method(D_METHOD("is_alive", "controller"), &Entity::is_alive);
    ClassDB::bind_method(D_METHOD("get_model", "controller"), &Entity::get_model);
    ClassDB::bind_method(D_METHOD("get_player", "controller"), &Entity::get_player);
    ClassDB::bind_method(D_METHOD("get_damage", "controller"), &Entity::get_damage);
    ClassDB::bind_method(D_METHOD("get_armor", "controller"), &Entity::get_armor);
    ClassDB::bind_method(D_METHOD("get_speed", "controller"), &Entity::get_speed);
    ClassDB::bind_method(D_METHOD("get_reload", "controller"), &Entity::get_reload);

    ADD_GROUP("Entity", "Entity_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "entity_handle"), "set_handle", "get_handle");
}

}
