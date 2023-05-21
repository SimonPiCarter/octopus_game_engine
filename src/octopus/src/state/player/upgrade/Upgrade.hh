#ifndef __Upgrade__
#define __Upgrade__

#include <string>
#include <vector>

#include "StepUpgradeGenerator.hh"
#include "state/model/requirements/Requirements.hh"
#include "utils/Fixed.hh"

namespace octopus
{

/// @brief a class that represent an upgrade
struct Upgrade
{
    /// @brief define an update that can be researched in buildings
    /// @param id_p id of the update (must be unique)
    /// @param generator_p the generator to be used when upgrade is done (ownership is taken)
    Upgrade(std::string const &id_p, StepUpgradeGenerator const *generator_p) : _id(id_p), _generator(generator_p) {}
    ~Upgrade() { delete _generator; }

    /// @brief requirements
    Requirements _requirements;

    /// @brief cost
    std::map<std::string, Fixed> _cost;

    /// @brief time required to produce the upgrade
    Fixed _productionTime;

    /// @brief id of the upgrade
    std::string _id;
    /// @brief step genrator to be used when upgrade is done
    StepUpgradeGenerator const * const _generator;

    bool _repeatable { false };
};

} // namespace octopus


#endif
