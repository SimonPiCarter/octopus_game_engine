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
#include "logger/Logger.hh"

namespace godot {

octopus::Handle castHandle(godot::EntityHandle const *handle_p)
{
    if(!handle_p)
    {
        throw std::logic_error("cannot get handle from null");
    }
    if(handle_p->get_index() < 0 || handle_p->get_revision() < 0)
    {
        throw std::logic_error("invalid cas from EntityHandle (negative index or revision)");
    }
    return octopus::Handle(handle_p->get_index(), handle_p->get_revision());
}

octopus::Handle castHandle(godot::Variant const &var_p)
{
    godot::EntityHandle const * handleEntity_l = godot::Node::cast_to<godot::EntityHandle>(var_p);
    return castHandle(handleEntity_l);
}

Entity::~Entity()
{}

void Entity::set_index(int index_p) {
    _index = index_p;
}

void Entity::set_revision(int revision_p) {
    _revision = revision_p;
}

EntityHandle * Entity::get_handle() const
{
    EntityHandle *handle_l = memnew(EntityHandle);
    handle_l->set_index(_index);
    handle_l->set_revision(_revision);
    return handle_l;
}

int Entity::get_index() const
{
    return _index;
}

int Entity::get_revision() const
{
    return _revision;
}

float Entity::get_hp(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->_hp);
}

float Entity::get_hp_max(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getHpMax());
}

Vector2 Entity::get_pos(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return Vector2(octopus::to_double(ent_l->_pos.x), octopus::to_double(ent_l->_pos.y));
}

bool Entity::is_alive(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_alive;
}

bool Entity::is_blueprint(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return false;
    }
    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->isBlueprint();
}

bool Entity::is_building(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._isBuilding;
}

bool Entity::is_built(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return true;
    }
    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->isBuilt();
}

bool Entity::is_resource(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._isResource;
}

bool Entity::is_unit(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._isUnit;
}

bool Entity::is_idle(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return !ent_l->getQueue().hasCommand();
}

int Entity::get_player(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_player;
}

int Entity::get_team(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    octopus::Player const *player_l = controller_p->getPlayer(ent_l->_player);
    return player_l->_team;
}

float Entity::get_ray(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->_model._ray);
}

String Entity::get_model(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._id.c_str();
}

float Entity::get_damage(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getDamageNoBonus());
}

float Entity::get_armor(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getArmor());
}

float Entity::get_speed(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getStepSpeed()*100);
}

float Entity::get_reload(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(100./ent_l->getFullReload());
}

bool Entity::has_rally_point(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return false;
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->_rallyPointActive;
}

Vector2 Entity::get_rally_point(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return Vector2(0,0);
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);
    return Vector2(octopus::to_double(building_l->_rallyPoint.x), octopus::to_double(building_l->_rallyPoint.y));
}

String Entity::get_resource_type(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isResource)
    {
        return "";
    }
    octopus::Resource const *res_l = static_cast<octopus::Resource const *>(ent_l);
    return res_l->_type.c_str();
}

float Entity::get_resource_quantity(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isResource)
    {
        return 0.;
    }
    octopus::Resource const *res_l = static_cast<octopus::Resource const *>(ent_l);
    return octopus::to_double(res_l->_resource);
}

TypedArray<godot::Buff> Entity::get_buffs(Controller const *controller_p) const
{
    TypedArray<godot::Buff> buffs_l;
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    for(auto &&pair_l : ent_l->_registeredBuff)
    {
        std::string const &name_l = pair_l.first;
        if(ent_l->_timeSinceBuff.at(name_l) < ent_l->_registeredBuff.at(name_l)._duration
        || ent_l->_registeredBuff.at(name_l)._duration == 0)
        {
            godot::Buff * buff_l = memnew(godot::Buff);
            buff_l->setName(name_l.c_str());
            buff_l->setTimeElapsed(ent_l->_timeSinceBuff.at(name_l));
            buff_l->setDuration(ent_l->_registeredBuff.at(name_l)._duration);

            buffs_l.push_back(buff_l);
        }
    }
    return buffs_l;
}

void EntityHandle::_bind_methods()
{
    UtilityFunctions::print("EntityHandle Buff methods");

    ClassDB::bind_method(D_METHOD("set_index", "index"), &EntityHandle::set_index);
    ClassDB::bind_method(D_METHOD("get_index"), &EntityHandle::get_index);

    ClassDB::bind_method(D_METHOD("set_revision", "revision"), &EntityHandle::set_revision);
    ClassDB::bind_method(D_METHOD("get_revision"), &EntityHandle::get_revision);

    ADD_GROUP("EntityHandle", "EntityHandle_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "index"), "set_index", "get_index");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "revision"), "set_revision", "get_revision");
}

void Buff::_bind_methods()
{
    UtilityFunctions::print("Binding Buff methods");

    ClassDB::bind_method(D_METHOD("setTimeElapsed", "time_elapsed"), &Buff::setTimeElapsed);
    ClassDB::bind_method(D_METHOD("getTimeElapsed"), &Buff::getTimeElapsed);

    ClassDB::bind_method(D_METHOD("setDuration", "duration"), &Buff::setDuration);
    ClassDB::bind_method(D_METHOD("getDuration"), &Buff::getDuration);

    ClassDB::bind_method(D_METHOD("setName", "name"), &Buff::setName);
    ClassDB::bind_method(D_METHOD("getName"), &Buff::getName);

    ADD_GROUP("Buff", "Buff_");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_elapsed"), "setTimeElapsed", "getTimeElapsed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "setDuration", "getDuration");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "setName", "getName");
}

void Entity::_bind_methods()
{
    UtilityFunctions::print("Binding Entity methods");

    ClassDB::bind_method(D_METHOD("set_index", "index"), &Entity::set_index);
    ClassDB::bind_method(D_METHOD("set_revision", "revision"), &Entity::set_revision);
    ClassDB::bind_method(D_METHOD("get_handle"), &Entity::get_handle);
    ClassDB::bind_method(D_METHOD("get_index"), &Entity::get_index);
    ClassDB::bind_method(D_METHOD("get_revision"), &Entity::get_revision);
    ClassDB::bind_method(D_METHOD("get_hp", "controller"), &Entity::get_hp);
    ClassDB::bind_method(D_METHOD("get_hp_max", "controller"), &Entity::get_hp_max);
    ClassDB::bind_method(D_METHOD("get_pos", "controller"), &Entity::get_pos);
    ClassDB::bind_method(D_METHOD("is_alive", "controller"), &Entity::is_alive);
    ClassDB::bind_method(D_METHOD("is_blueprint", "controller"), &Entity::is_blueprint);
    ClassDB::bind_method(D_METHOD("is_building", "controller"), &Entity::is_building);
    ClassDB::bind_method(D_METHOD("is_built", "controller"), &Entity::is_built);
    ClassDB::bind_method(D_METHOD("is_resource", "controller"), &Entity::is_resource);
    ClassDB::bind_method(D_METHOD("is_unit", "controller"), &Entity::is_unit);
    ClassDB::bind_method(D_METHOD("is_idle", "controller"), &Entity::is_idle);
    ClassDB::bind_method(D_METHOD("get_ray", "controller"), &Entity::get_ray);
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
    ClassDB::bind_method(D_METHOD("get_buffs", "controller"), &Entity::get_buffs);

    ADD_GROUP("Entity", "Entity_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "entity_index"), "set_index", "get_index");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "entity_revision"), "set_revision", "get_revision");
}

}
