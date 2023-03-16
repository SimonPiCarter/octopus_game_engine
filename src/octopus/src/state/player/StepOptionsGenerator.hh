#ifndef __StepOptionsGenerator__
#define __StepOptionsGenerator__

#include <vector>

namespace octopus
{
class Steppable;

/// @brief this simple class represents an steppable generator option
/// An option is a list of steps to be applied if the option
/// is popped from the option map for a player
class StepOptionsGenerator
{
public:
    /// @brief Create a new copy of this generator
    /// this is required to ease up memory handling between steps and states for options handling
    /// @return a newly created copy of this genertor
    virtual StepOptionsGenerator* newCopy() const = 0;

    /// @brief get internal steppable for the given option
    /// @param options_p the option index
    /// @return a vector of steppables (owner is this generator)
    virtual std::vector<Steppable *> getSteppables(unsigned long options_p) const = 0;
    /// @brief Return the number of available options for this generator
    virtual unsigned long getNumOptions() const = 0;
};

} // namespace octopus


#endif