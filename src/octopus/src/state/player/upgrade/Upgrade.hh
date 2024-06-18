#ifndef __Upgrade__
#define __Upgrade__

#include <map>
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
    Upgrade(std::string const &id_p, StepUpgradeGenerator const *generator_p);
    ~Upgrade() { delete _generator; }

    void addLevel();

    /// @brief requirements
    std::vector<Requirements> _requirements;

    /// @brief cost
    std::vector< std::map<std::string, Fixed> > _cost;

    /// @brief time required to produce the upgrade
    std::vector<int> _productionTime;

    /// @brief id of the upgrade
    std::string _id;
    /// @brief step genrator to be used when upgrade is done
    StepUpgradeGenerator const * const _generator;
};

/// @brief Helper to get the upgrade cost based on its level
/// @param upgrade_p upgrade
/// @param level_p level
/// @return a cost map
std::map<std::string, Fixed> const &getCost(Upgrade const &upgrade_p, unsigned long level_p);

/// @brief Helper to get the requirements based on its level
/// @param upgrade_p upgrade
/// @param level_p level
/// @return a requirement
Requirements const &getRequirements(Upgrade const &upgrade_p, unsigned long level_p);

/// @brief helper to get the production time
/// @param upgrade_p upgrade
/// @param level_p level
/// @return a step duration for the production time
int getProductionTime(Upgrade const &upgrade_p, unsigned long level_p);

unsigned long getMaxLevel(Upgrade const &upgrade_p);

} // namespace octopus


#endif
