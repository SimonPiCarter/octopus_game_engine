#ifndef __Godoctopus_ModelUpgrade__
#define __Godoctopus_ModelUpgrade__

#include <string>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"

namespace octopus
{
    struct Upgrade;
} // namespace octopus


namespace godot {

class Controller;

class ModelUpgrade : public Node {
    GDCLASS(ModelUpgrade, Node)

public:
    ~ModelUpgrade();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    /// @brief initialise ModelUpgrade with all mandatory params for octopus engine
    /// every following method will be no op if this was not called before
    /// @param id_p name or id (unique) of the upgrade
    /// @param time_p step time required for the upgrade to complete
    void init(String const &id_p, float time_p);

    /// @brief register the ModelUpgrade in the library, every modification after this call will be a no op operation
    /// since ModelUpgrade is copied in the library
    /// @param controller_p pointer to the controller to have the library
    void saveUpgrade(Controller *controller_p, String const &name_p);

    void add_cost(String const &res_p, int qty_p);
    // requirements
    void add_no_building_requirements(String const &name_p);
    void add_building_requirements(String const &name_p);
    void add_upgrade_lvl_requirements_min(String const &name_p, int level_p);
    void add_upgrade_lvl_requirements_max(String const &name_p, int level_p);

private:
    octopus::Upgrade * _upgrade {nullptr};
};

}

#endif //__Godoctopus_ModelUpgrade__
