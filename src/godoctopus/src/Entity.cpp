#include "Entity.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Building.hh"
#include "state/entity/Resource.hh"

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

bool Entity::is_blueprint(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    if(!ent_l->_model._isBuilding)
    {
        return false;
    }
    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->isBlueprint();
}

bool Entity::is_building(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_model._isBuilding;
}

bool Entity::is_built(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    if(!ent_l->_model._isBuilding)
    {
        return true;
    }
    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->isBuilt();
}

bool Entity::is_resource(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_model._isResource;
}

bool Entity::is_unit(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_model._isUnit;
}

int Entity::get_player(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    return ent_l->_player;
}

int Entity::get_team(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    octopus::Player const *player_l = controller_p->getPlayer(ent_l->_player);
    return player_l->_team;
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

bool Entity::has_rally_point(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    if(!ent_l->_model._isBuilding)
    {
        return false;
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->_rallyPointActive;
}

Vector2 Entity::get_rally_point(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    if(!ent_l->_model._isBuilding)
    {
        return Vector2(0,0);
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);
    return Vector2(octopus::to_double(building_l->_rallyPoint.x), octopus::to_double(building_l->_rallyPoint.y));
}

String Entity::get_resource_type(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    if(!ent_l->_model._isResource)
    {
        return "";
    }
    octopus::Resource const *res_l = static_cast<octopus::Resource const *>(ent_l);
    return octopus::to_string(res_l->_type).c_str();
}

float Entity::get_resource_quantity(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(_handle);
    if(!ent_l->_model._isResource)
    {
        return 0.;
    }
    octopus::Resource const *res_l = static_cast<octopus::Resource const *>(ent_l);
    return res_l->_resource;
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
    ClassDB::bind_method(D_METHOD("is_blueprint", "controller"), &Entity::is_blueprint);
    ClassDB::bind_method(D_METHOD("is_building", "controller"), &Entity::is_building);
    ClassDB::bind_method(D_METHOD("is_built", "controller"), &Entity::is_built);
    ClassDB::bind_method(D_METHOD("is_resource", "controller"), &Entity::is_resource);
    ClassDB::bind_method(D_METHOD("is_unit", "controller"), &Entity::is_unit);
    ClassDB::bind_method(D_METHOD("get_model", "controller"), &Entity::get_model);
    ClassDB::bind_method(D_METHOD("get_player", "controller"), &Entity::get_player);
    ClassDB::bind_method(D_METHOD("get_team", "controller"), &Entity::get_team);
    ClassDB::bind_method(D_METHOD("get_damage", "controller"), &Entity::get_damage);
    ClassDB::bind_method(D_METHOD("get_armor", "controller"), &Entity::get_armor);
    ClassDB::bind_method(D_METHOD("get_speed", "controller"), &Entity::get_speed);
    ClassDB::bind_method(D_METHOD("get_reload", "controller"), &Entity::get_reload);
    ClassDB::bind_method(D_METHOD("has_rally_point", "controller"), &Entity::has_rally_point);
    ClassDB::bind_method(D_METHOD("get_rally_point", "controller"), &Entity::get_rally_point);
    ClassDB::bind_method(D_METHOD("get_resource_type", "controller"), &Entity::get_resource_type);
    ClassDB::bind_method(D_METHOD("get_resource_quantity", "controller"), &Entity::get_resource_quantity);

    ADD_GROUP("Entity", "Entity_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "entity_handle"), "set_handle", "get_handle");
}

}
