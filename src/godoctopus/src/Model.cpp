#include "Model.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/player/Player.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace godot {

Model::~Model()
{
    delete _model;
    delete _buildingModel;
    delete _unitModel;
}

void Model::init(String const &type_p, float radius_p, int los_p, float stepSpeed_p, int hp_p)
{
    // reset internal pointers
    delete _model;
    delete _buildingModel;
    delete _unitModel;
    _model = nullptr;
    _buildingModel = nullptr;
    _unitModel = nullptr;

    std::string typeId_l(type_p.utf8().get_data());

    if("Unit" == typeId_l || "Builder" == typeId_l)
    {
        _unitModel = new octopus::UnitModel(false, radius_p, stepSpeed_p, hp_p);
        _model = _unitModel;
        _model->_isUnit = true;
        _model->_isBuilder = "Builder" == typeId_l;
    }
    else if("Building" == typeId_l)
    {
        _buildingModel = new octopus::BuildingModel(true, radius_p, hp_p);
        _model = _buildingModel;
        _model->_isBuilding = true;
    }
    else if("Resource" == typeId_l)
    {
        _model = new octopus::EntityModel(true, radius_p, stepSpeed_p, hp_p);
        _model->_isResource = true;
        _model->_invulnerable = true;
    }
    else
    {
        UtilityFunctions::print("ignored init because type was not valid from Unit, Builder, Building, Resource");
        return;
    }
    _model->_lineOfSight = los_p;
}

void Model::save(Controller *controller_p, String const &name_p)
{
    if(_buildingModel)
    {
	    controller_p->getLib().registerBuildingModel(name_p.utf8().get_data(), *_buildingModel);
    }
    else if(_unitModel)
    {
	    controller_p->getLib().registerUnitModel(name_p.utf8().get_data(), *_unitModel);
    }
    else
    {
	    controller_p->getLib().registerEntityModel(name_p.utf8().get_data(), *_model);
    }
}

/////////////////
//    Entity   //
/////////////////
void Model::add_cost(String const &res_p, int qty_p)
{
    std::string resType_l(res_p.utf8().get_data());
    _model->_cost[resType_l] = qty_p;
}

void Model::set_attack_info(int damage_p, int armor_p, float range_p, int windup_p, int fullReload_p)
{
    _model->_damage = damage_p;
    _model->_armor = armor_p;
    _model->_range = range_p;
    _model->_windup = windup_p;
    _model->_fullReload = fullReload_p;
}

void Model::add_bonus_damage(String const &model_p, int bonus_p)
{
    std::string modelType_l(model_p.utf8().get_data());
    _model->_bonusDamage[modelType_l] = bonus_p;
}

void Model::set_aoe_modifier(float ratio_p, float range_p, bool friendlyFire_p)
{
    _model->_defaultAttackMod = octopus::AoEModifier(ratio_p, range_p, friendlyFire_p);
}
void Model::set_chaining_modifier(int delay_p, int chains_p, float ratio_p, float range_p)
{
    _model->_defaultAttackMod = octopus::ChainingModifier(delay_p, chains_p, ratio_p, range_p);
}
void Model::set_dot_modifier(int tickrate_p, int nb_ticks_p, float dmg_p)
{
    _model->_defaultAttackMod = octopus::DotModifier(tickrate_p, nb_ticks_p, dmg_p);
}
void Model::set_lifesteal_modifier(float ratio_p)
{
    _model->_defaultAttackMod = octopus::LifeStealModifier(ratio_p);
}
void Model::set_selfdamage_modifier(float dmg_p, float ratio_cur_hp_p, float ratio_max_hp_p, bool can_kill_p)
{
    _model->_defaultAttackMod = octopus::SelfDamageModifier(dmg_p, ratio_cur_hp_p, ratio_max_hp_p, can_kill_p);
}

void Model::add_no_building_requirements(String const &name_p)
{
    _model->_requirements._noBuildings.insert(name_p.utf8().get_data());
}

void Model::add_building_requirements(String const &name_p)
{
    _model->_requirements._buildings.insert(name_p.utf8().get_data());
}

void Model::add_techonology_requirements(String const &name_p)
{
    _model->_requirements._technologies.insert(name_p.utf8().get_data());
}

void Model::add_div_lvl_requirements(String const &name_p, int level_p)
{
    _model->_requirements._divLvl[name_p.utf8().get_data()] = level_p;
}

/////////////////
//   Building  //
/////////////////
void Model::add_unit_produced(Controller *controller_p, String const &unit_p)
{
    if(!_buildingModel)
    {
        UtilityFunctions::print("ignored add_unit_produced for ", unit_p, " because not a building model");
        return;
    }

    std::string unitType_l(unit_p.utf8().get_data());

    if(!controller_p->getLib().hasUnitModel(unitType_l))
    {
        UtilityFunctions::print("Could not add_unit_produced, unit not found : ", unit_p);
        return;
    }

    _buildingModel->_unitModels.push_back(&controller_p->getLib().getUnitModel(unitType_l));
}

void Model::set_deposit(String const &res_p, bool deposit_p)
{
    if(!_buildingModel)
    {
        UtilityFunctions::print("ignored set_deposit because not a building model");
        return;
    }

    std::string resType_l(res_p.utf8().get_data());

    _buildingModel->_deposit[resType_l] = deposit_p;
}

void Model::set_building_time(int time_p)
{
    if(!_buildingModel)
    {
        UtilityFunctions::print("ignored set_building_time because not a building model");
        return;
    }

    _buildingModel->_buildingTime = time_p;
}

/////////////////
//     Unit    //
/////////////////
void Model::set_production_time(int time_p)
{
    if(!_unitModel)
    {
        UtilityFunctions::print("ignored set_production_time because not a unit model");
        return;
    }

    _unitModel->_productionTime = time_p;
}
void Model::set_harvest_time(int time_p)
{
    if(!_unitModel)
    {
        UtilityFunctions::print("ignored set_harvest_time because not a unit model");
        return;
    }

    _unitModel->_timeToHarvest = time_p;
}
void Model::set_harvest_quantity(String const &res_p, int quantity_p)
{
    if(!_unitModel)
    {
        UtilityFunctions::print("ignored set_harvest_quantity because not a unit model");
        return;
    }

    std::string resType_l(res_p.utf8().get_data());

    _unitModel->_maxQuantity[resType_l] = quantity_p;
}

void Model::_bind_methods()
{
    UtilityFunctions::print("Binding Model methods");

    // general
    ClassDB::bind_method(D_METHOD("init", "type", "radius", "los", "stepSpeed", "hp"), &Model::init);
    ClassDB::bind_method(D_METHOD("save", "controller", "name"), &Model::save);

    // Entity
    ClassDB::bind_method(D_METHOD("add_cost", "res", "qty"), &Model::add_cost);
    ClassDB::bind_method(D_METHOD("set_attack_info", "damage", "armor", "range", "windup", "fullReload"), &Model::set_attack_info);
    ClassDB::bind_method(D_METHOD("add_bonus_damage", "model", "bonus"), &Model::add_bonus_damage);

    ClassDB::bind_method(D_METHOD("set_aoe_modifier", "ratio", "range", "friendlyFire"), &Model::set_aoe_modifier);
    ClassDB::bind_method(D_METHOD("set_chaining_modifier", "delay", "chains", "ratio", "range"), &Model::set_chaining_modifier);
    ClassDB::bind_method(D_METHOD("set_dot_modifier", "tickrate", "nb_ticks", "dmg"), &Model::set_dot_modifier);
    ClassDB::bind_method(D_METHOD("set_lifesteal_modifier", "ratio"), &Model::set_lifesteal_modifier);
    ClassDB::bind_method(D_METHOD("set_selfdamage_modifier", "dmg", "ratio_cur_hp", "ratio_max_hp", "can_kill"), &Model::set_selfdamage_modifier);

    ClassDB::bind_method(D_METHOD("add_no_building_requirements", "name"), &Model::add_no_building_requirements);
    ClassDB::bind_method(D_METHOD("add_building_requirements", "name"), &Model::add_building_requirements);
    ClassDB::bind_method(D_METHOD("add_techonology_requirements", "name"), &Model::add_techonology_requirements);
    ClassDB::bind_method(D_METHOD("add_div_lvl_requirements", "name", "level"), &Model::add_div_lvl_requirements);

    // Building
    ClassDB::bind_method(D_METHOD("add_unit_produced", "controller", "unit"), &Model::add_unit_produced);
    ClassDB::bind_method(D_METHOD("set_deposit", "res", "deposit"), &Model::set_deposit);
    ClassDB::bind_method(D_METHOD("set_building_time", "time"), &Model::set_building_time);

    /// Unit
    ClassDB::bind_method(D_METHOD("set_production_time", "time"), &Model::set_production_time);
    ClassDB::bind_method(D_METHOD("set_harvest_time", "time"), &Model::set_harvest_time);
    ClassDB::bind_method(D_METHOD("set_harvest_quantity", "res", "quantity"), &Model::set_harvest_quantity);

    ADD_GROUP("Model", "Model_");
}

}
