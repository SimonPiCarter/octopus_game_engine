#ifndef __Godoctopus_Model__
#define __Godoctopus_Model__

#include <map>
#include <string>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"
#include "state/model/requirements/Requirements.hh"
#include "utils/Fixed.hh"

namespace octopus
{
    struct EntityModel;
    struct BuildingModel;
    struct UnitModel;
} // namespace octopus


namespace godot {

class Controller;

class Model : public Node {
    GDCLASS(Model, Node)

public:
    ~Model();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    /// @brief initialise model with all mandatory params for octopus engine
    /// every following method will failed if this was not called before
    /// @param type_p type of the model can be one of the following : "Unit", "Building", "Resource", "Builder"
    /// @param radius_p radius of the entity in the collision scheme
    /// @param los_p line of sight of the entity
    /// @param stepSteep_p movement per step of the entity
    /// @param hp_p hit point of the entity
    void init(String const &type_p, float radius_p, int los_p, float stepSpeed_p, int hp_p);

    /// @brief register the model in the library, every modification after this call will be a no op operation
    /// since model is copied in the library
    /// @param controller_p pointer to the controller to have the library
    /// @param name_p name of the model, must be unique
    void save(Controller *controller_p, String const &name_p);

    /////////////////
    //    Entity   //
    /////////////////
    // cost
    void add_cost(String const &res_p, int qty_p);
    // damage
    void set_attack_info(int damage_p, int armor_p, float range_p, int windup_p, int fullReload_p);
    void add_bonus_damage(String const &model_p, int bonus_p);
    // attack_modifiers
    void set_aoe_modifier(float ratio_p, float range_p, bool friendlyFire_p);
    void set_chaining_modifier(int delay_p, int chains_p, float ratio_p, float range_p);
    void set_dot_modifier(int tickrate_p, int nb_ticks_p, float dmg_p);
    void set_lifesteal_modifier(float ratio_p);
    void set_selfdamage_modifier(float dmg_p, float ratio_cur_hp_p, float ratio_max_hp_p, bool can_kill_p);
    // requirements
    void add_no_building_requirements(String const &name_p);
    void add_building_requirements(String const &name_p);
    void add_upgrade_lvl_requirements_min(String const &name_p, int level_p);
    void add_upgrade_lvl_requirements_max(String const &name_p, int level_p);


    /////////////////
    //   Building  //
    /////////////////
    void add_unit_produced(Controller *controller_p, String const &unit_p);
    void add_upgrade_produced(Controller *controller_p, String const &upgrade_p);
    void set_deposit(String const &res_p, bool deposit_p);
    void set_building_time(int time_p);

    /////////////////
    //     Unit    //
    /////////////////
    void set_production_time(int time_p);
    void set_harvest_time(int time_p);
    void set_harvest_quantity(String const &res_p, int quantity_p);

private:
    octopus::EntityModel *_model { nullptr };
    octopus::BuildingModel *_buildingModel { nullptr };
    octopus::UnitModel *_unitModel { nullptr };
};

/// @brief object that can be loaded from either entity or upgrades
class ModelView : public Node {
    GDCLASS(ModelView, Node)

public:
    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    /// @brief Initialize ModelView with the given model
    /// @param controller_p
    /// @param name_p
    void init(Controller const *controller_p, int producer_p, String const &name_p);

    // cost
    TypedArray<String> get_cost_resources_names() const;
    float get_cost_resource_quantity(String const &res_p) const;

    // requirements
    TypedArray<String> get_no_building_requirements() const;
    TypedArray<String> get_building_requirements() const;
    TypedArray<String> get_upgrade_requirements_min() const;
    int get_upgrade_requirements_min_lvl(String const &up_p) const;
    TypedArray<String> get_upgrade_requirements_max() const;
    int get_upgrade_requirements_max_lvl(String const &up_p) const;

    // time
    float get_production_time() const;

private:
    /// @brief requirements
    octopus::Requirements _requirements;

    /// @brief cost
    std::map<std::string, octopus::Fixed> _cost;

    float _productionTime = 0;
};

}

#endif //__Godoctopus_Model__
