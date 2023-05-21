#include "ModelUpgrade.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/player/upgrade/Upgrade.hh"

namespace godot {

ModelUpgrade::~ModelUpgrade()
{
    delete _upgrade;
}

void ModelUpgrade::init(String const &id_p, float time_p)
{
    // reset internal pointers
    delete _upgrade;
    std::string typeId_l(id_p.utf8().get_data());
    _upgrade = new octopus::Upgrade(typeId_l, nullptr);
    _upgrade->_productionTime = time_p;
}

void ModelUpgrade::saveUpgrade(Controller *controller_p, String const &name_p)
{
    if(_upgrade)
    {
        std::string typeId_l(name_p.utf8().get_data());
        if(typeId_l != _upgrade->_id)
        {
            UtilityFunctions::print("Did not save ", name_p, " becase name was incoherent with id givne in init");
            return;
        }
        controller_p->getLib().registerUpgrade(typeId_l, _upgrade);
        _upgrade = nullptr;
    }
}

void ModelUpgrade::add_cost(String const &res_p, int qty_p)
{
    if(_upgrade)
    {
        std::string resType_l(res_p.utf8().get_data());
        _upgrade->_cost[resType_l] = qty_p;
    }
}

void ModelUpgrade::add_no_building_requirements(String const &name_p)
{
    if(_upgrade)
    {
        _upgrade->_requirements._noBuildings.insert(name_p.utf8().get_data());
    }
}

void ModelUpgrade::add_building_requirements(String const &name_p)
{
    if(_upgrade)
    {
        _upgrade->_requirements._buildings.insert(name_p.utf8().get_data());
    }
}

void ModelUpgrade::add_upgrade_lvl_requirements_min(String const &name_p, int level_p)
{
    if(_upgrade)
    {
        _upgrade->_requirements._upgradeLvl[name_p.utf8().get_data()] = level_p;
    }
}

void ModelUpgrade::add_upgrade_lvl_requirements_max(String const &name_p, int level_p)
{
    if(_upgrade)
    {
        _upgrade->_requirements._upgradeLvlMax[name_p.utf8().get_data()] = level_p;
    }
}

void ModelUpgrade::_bind_methods()
{
    UtilityFunctions::print("Binding ModelUpgrade methods");

    // general
    ClassDB::bind_method(D_METHOD("init", "name", "time"), &ModelUpgrade::init);
    ClassDB::bind_method(D_METHOD("saveUpgrade", "controller", "name"), &ModelUpgrade::saveUpgrade);

    ClassDB::bind_method(D_METHOD("add_cost", "res", "qty"), &ModelUpgrade::add_cost);
    ClassDB::bind_method(D_METHOD("add_no_building_requirements", "name"), &ModelUpgrade::add_no_building_requirements);
    ClassDB::bind_method(D_METHOD("add_building_requirements", "name"), &ModelUpgrade::add_building_requirements);
    ClassDB::bind_method(D_METHOD("add_upgrade_lvl_requirements_min", "name", "level"), &ModelUpgrade::add_upgrade_lvl_requirements_min);
    ClassDB::bind_method(D_METHOD("add_upgrade_lvl_requirements_max", "name", "level"), &ModelUpgrade::add_upgrade_lvl_requirements_max);

    ADD_GROUP("ModelUpgrade", "ModelUpgrade_");
}

}
