#ifndef __StepUpgradeNoOpGenerator__
#define __StepUpgradeNoOpGenerator__

#include "StepUpgradeGenerator.hh"

namespace octopus
{

/// @brief a class that allow upgrade with no op (just unlock requirements usually)
class StepUpgradeNoOpGenerator : public StepUpgradeGenerator
{
public:
    ~StepUpgradeNoOpGenerator();

    /// @brief Create a new copy of this generator
    /// this is required to ease up memory handling between steps and states for options handling
    /// @return a newly created copy of this genertor
    virtual StepUpgradeGenerator* newCopy() const override;

    /// @brief get internal steppable for the given option
    /// @param player_p the player for the steps to be applied
    /// @return a vector of steppables (owner is given away)
    virtual std::vector<Steppable *> getSteppables(unsigned long player_p, unsigned long) const override;
};

} // namespace octopus


#endif
