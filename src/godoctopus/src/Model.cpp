#include "Model.h"
#include "Controller.h"
#include "TooltipLoader.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/player/Player.hh"
#include "state/player/upgrade/Upgrade.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/ResourceModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace godot {

Model::~Model()
{
    delete _model;
    delete _buildingModel;
    delete _resourceModel;
    delete _unitModel;
}

void Model::init(String const &type_p, float radius_p, int los_p, float stepSpeed_p, int hp_p)
{
    // reset internal pointers
    delete _model;
    delete _buildingModel;
    delete _resourceModel;
    delete _unitModel;
    _model = nullptr;
    _buildingModel = nullptr;
	_resourceModel = nullptr;
    _unitModel = nullptr;

    std::string typeId_l(type_p.utf8().get_data());

    if("Unit" == typeId_l || "Builder" == typeId_l)
    {
        _unitModel = new octopus::UnitModel(false, radius_p, stepSpeed_p, hp_p);
        _model = _unitModel;
        _model->_isBuilder = "Builder" == typeId_l;
    }
    else if("Building" == typeId_l)
    {
        _buildingModel = new octopus::BuildingModel(true, radius_p, hp_p);
        _model = _buildingModel;
    }
    else if("Resource" == typeId_l)
    {
        _resourceModel = new octopus::ResourceModel(radius_p, "bloc", hp_p);
        _model = _resourceModel;
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
        // add spawn
        _buildingModel->_productionOutput = octopus::Vector(numeric::ceil(_buildingModel->_ray), 0);
	    controller_p->getLib().registerBuildingModel(name_p.utf8().get_data(), *_buildingModel);
    }
    else if(_resourceModel)
    {
	    controller_p->getLib().registerResourceModel(name_p.utf8().get_data(), *_resourceModel);
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

void Model::set_projectile(bool projectile_p)
{
    _model->_projectile = projectile_p;
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

void Model::add_upgrade_lvl_requirements_min(String const &name_p, int level_p)
{
    _model->_requirements._upgradeLvl[name_p.utf8().get_data()] = level_p;
}

void Model::add_upgrade_lvl_requirements_max(String const &name_p, int level_p)
{
    _model->_requirements._upgradeLvlMax[name_p.utf8().get_data()] = level_p;
}

void Model::set_invulnerable(bool invulnerable_p)
{
	_model->_invulnerable = invulnerable_p;
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

void Model::add_upgrade_produced(Controller *controller_p, String const &upgrade_p)
{
    if(!_buildingModel)
    {
        UtilityFunctions::print("ignored add_upgrade_produced for ", upgrade_p, " because not a building model");
        return;
    }

    std::string upgradeType_l(upgrade_p.utf8().get_data());

    if(!controller_p->getLib().hasUpgrade(upgradeType_l))
    {
        UtilityFunctions::print("Could not add_upgrade_produced, upgrade not found : ", upgrade_p);
        return;
    }

    _buildingModel->_upgrades.push_back(&controller_p->getLib().getUpgrade(upgradeType_l));
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

////////////////////
//     Resource   //
////////////////////
void Model::set_type(String const &resType_p)
{
    if(!_resourceModel)
    {
        UtilityFunctions::print("ignored set_type because not a resource model");
        return;
    }

    std::string resType_l(resType_p.utf8().get_data());

    _resourceModel->_type = resType_l;
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
    ClassDB::bind_method(D_METHOD("set_projectile", "projectile"), &Model::set_projectile);

    ClassDB::bind_method(D_METHOD("set_aoe_modifier", "ratio", "range", "friendlyFire"), &Model::set_aoe_modifier);
    ClassDB::bind_method(D_METHOD("set_chaining_modifier", "delay", "chains", "ratio", "range"), &Model::set_chaining_modifier);
    ClassDB::bind_method(D_METHOD("set_dot_modifier", "tickrate", "nb_ticks", "dmg"), &Model::set_dot_modifier);
    ClassDB::bind_method(D_METHOD("set_lifesteal_modifier", "ratio"), &Model::set_lifesteal_modifier);
    ClassDB::bind_method(D_METHOD("set_selfdamage_modifier", "dmg", "ratio_cur_hp", "ratio_max_hp", "can_kill"), &Model::set_selfdamage_modifier);

    ClassDB::bind_method(D_METHOD("add_no_building_requirements", "name"), &Model::add_no_building_requirements);
    ClassDB::bind_method(D_METHOD("add_building_requirements", "name"), &Model::add_building_requirements);
    ClassDB::bind_method(D_METHOD("add_upgrade_lvl_requirements_min", "name", "level"), &Model::add_upgrade_lvl_requirements_min);
    ClassDB::bind_method(D_METHOD("add_upgrade_lvl_requirements_max", "name", "level"), &Model::add_upgrade_lvl_requirements_max);

    ClassDB::bind_method(D_METHOD("set_invulnerable", "invulnerable"), &Model::set_invulnerable);

    // Building
    ClassDB::bind_method(D_METHOD("add_unit_produced", "controller", "unit"), &Model::add_unit_produced);
    ClassDB::bind_method(D_METHOD("add_upgrade_produced", "controller", "unit"), &Model::add_upgrade_produced);
    ClassDB::bind_method(D_METHOD("set_deposit", "res", "deposit"), &Model::set_deposit);
    ClassDB::bind_method(D_METHOD("set_building_time", "time"), &Model::set_building_time);

    /// Unit
    ClassDB::bind_method(D_METHOD("set_production_time", "time"), &Model::set_production_time);
    ClassDB::bind_method(D_METHOD("set_harvest_time", "time"), &Model::set_harvest_time);
    ClassDB::bind_method(D_METHOD("set_harvest_quantity", "res", "quantity"), &Model::set_harvest_quantity);

    ADD_GROUP("Model", "Model_");
}

void ModelView::init(Controller const *controller_p, EntityHandle const * producer_p, String const &name_p)
{
    std::string name_l = name_p.utf8().get_data();
    octopus::Entity const & entProducer_l = *controller_p->getEntity(castHandle(producer_p));
    octopus::Player const & player_l = *controller_p->getPlayer(entProducer_l._player);
    float productionSpeed_l = entProducer_l.getProduction().to_double();
    float productionTime_l = 0;
    if(controller_p->getLib().hasEntityModel(name_l))
    {
        _requirements = controller_p->getLib().getEntityModel(name_l)._requirements;
        _cost = controller_p->getLib().getEntityModel(name_l)._cost;

        if(controller_p->getLib().hasUnitModel(name_l))
        {
            productionTime_l = octopus::to_double(getProductionTime(controller_p->getLib().getUnitModel(name_l), player_l));
            _cost = getCost(controller_p->getLib().getUnitModel(name_l), player_l);
        }

        if(controller_p->getLib().hasBuildingModel(name_l))
        {
            productionTime_l = controller_p->getLib().getBuildingModel(name_l)._buildingTime;
        }
    }
    else if(controller_p->getLib().hasUpgrade(name_l))
    {
        unsigned long upgradeLvl_l = getUpgradeLvl(player_l, name_l);
        octopus::Upgrade const &upgrade_l = controller_p->getLib().getUpgrade(name_l);
        _requirements = octopus::getRequirements(upgrade_l, upgradeLvl_l);
        _cost = octopus::getCost(upgrade_l, upgradeLvl_l);
        productionTime_l = octopus::getProductionTime(upgrade_l, upgradeLvl_l);
    }
    _productionTime = productionTime_l / productionSpeed_l;
    // initialize tool tip parameters
    _tooltipParameters = loadTooltipParameter(controller_p, name_l);
}

TypedArray<String> ModelView::get_cost_resources_names() const
{
    TypedArray<String> resourcesNames_l;
    for(auto &&pair_l : _cost)
    {
        resourcesNames_l.push_back(pair_l.first.c_str());
    }
    return resourcesNames_l;
}

float ModelView::get_cost_resource_quantity(String const &res_p) const
{
    std::string resType_l(res_p.utf8().get_data());
    return octopus::to_double(_cost.at(resType_l));
}

TypedArray<String> ModelView::get_no_building_requirements() const
{
    TypedArray<String> req_l;
    for(std::string const &build_l : _requirements._noBuildings)
    {
        req_l.push_back(build_l.c_str());
    }
    return req_l;
}

TypedArray<String> ModelView::get_building_requirements() const
{
    TypedArray<String> req_l;
    for(std::string const &build_l : _requirements._buildings)
    {
        req_l.push_back(build_l.c_str());
    }
    return req_l;
}

TypedArray<String> ModelView::get_upgrade_requirements_min() const
{
    TypedArray<String> req_l;
    for(auto &&pair_l : _requirements._upgradeLvl)
    {
        req_l.push_back(pair_l.first.c_str());
    }
    return req_l;
}

int ModelView::get_upgrade_requirements_min_lvl(String const &up_p) const
{
    std::string up_l(up_p.utf8().get_data());
    return _requirements._upgradeLvl.at(up_l);
}

TypedArray<String> ModelView::get_upgrade_requirements_max() const
{
    TypedArray<String> req_l;
    for(auto &&pair_l : _requirements._upgradeLvlMax)
    {
        req_l.push_back(pair_l.first.c_str());
    }
    return req_l;
}

int ModelView::get_upgrade_requirements_max_lvl(String const &up_p) const
{
    std::string up_l(up_p.utf8().get_data());
    return _requirements._upgradeLvlMax.at(up_l);
}

TypedArray<String> ModelView::get_tooltip_parameters() const
{
    TypedArray<String> params_l;
    for(std::string const &str_l : _tooltipParameters)
    {
        params_l.push_back(str_l.c_str());
    }
    return params_l;
}

float ModelView::get_production_time() const
{
    return _productionTime;
}


void ModelView::_bind_methods()
{
    UtilityFunctions::print("Binding ModelView methods");

    // general
    ClassDB::bind_method(D_METHOD("init", "controller", "producer", "name"), &ModelView::init);

    //cost
    ClassDB::bind_method(D_METHOD("get_cost_resources_names"), &ModelView::get_cost_resources_names);
    ClassDB::bind_method(D_METHOD("get_cost_resource_quantity", "res"), &ModelView::get_cost_resource_quantity);

    // req
    ClassDB::bind_method(D_METHOD("get_no_building_requirements"), &ModelView::get_no_building_requirements);
    ClassDB::bind_method(D_METHOD("get_building_requirements"), &ModelView::get_building_requirements);
    ClassDB::bind_method(D_METHOD("get_upgrade_requirements_min"), &ModelView::get_upgrade_requirements_min);
    ClassDB::bind_method(D_METHOD("get_upgrade_requirements_min_lvl", "up"), &ModelView::get_upgrade_requirements_min_lvl);
    ClassDB::bind_method(D_METHOD("get_upgrade_requirements_max"), &ModelView::get_upgrade_requirements_max);
    ClassDB::bind_method(D_METHOD("get_upgrade_requirements_max_lvl", "up"), &ModelView::get_upgrade_requirements_max_lvl);
    ClassDB::bind_method(D_METHOD("get_tooltip_parameters"), &ModelView::get_tooltip_parameters);

    // time
    ClassDB::bind_method(D_METHOD("get_production_time"), &ModelView::get_production_time);

    ADD_GROUP("ModelView", "ModelView_");
}

}
