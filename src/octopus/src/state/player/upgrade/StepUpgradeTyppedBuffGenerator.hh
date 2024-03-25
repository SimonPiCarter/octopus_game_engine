#ifndef __StepUpgradeTyppedBuffGenerator__
#define __StepUpgradeTyppedBuffGenerator__

#include <string>
#include <vector>

#include "StepUpgradeGenerator.hh"
#include "state/entity/buff/TimedBuff.hh"

namespace octopus
{

/// @brief a class that allow upgrade to apply a buff
/// for a player and all models given
class StepUpgradeTyppedBuffGenerator : public StepUpgradeGenerator
{
public:
    StepUpgradeTyppedBuffGenerator(TimedBuff const &buff_p, std::vector<std::string> const &models_p);
    StepUpgradeTyppedBuffGenerator(std::vector<TimedBuff> const &buffs_p, std::vector<std::string> const &models_p);
    ~StepUpgradeTyppedBuffGenerator();

    /// @brief Create a new copy of this generator
    /// this is required to ease up memory handling between steps and states for options handling
    /// @return a newly created copy of this genertor
    virtual StepUpgradeGenerator* newCopy() const override;

    /// @brief get internal steppable for the given option
    /// @param player_p the player for the steps to be applied
    /// @return a vector of steppables (owner is given away)
    virtual std::vector<Steppable *> getSteppables(unsigned long player_p, unsigned long) const override;

protected:
    std::vector<TimedBuff> _buffs;
    std::vector<std::string> _models;
};

} // namespace octopus


#endif
