#ifndef __CustomStep__
#define __CustomStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
/// @brief this is a place holder step to allow step overrinding
class CustomStep : public Steppable
{
public:
    CustomStep() {}
    virtual void visit(SteppableVisitor * visitor_p) const override
    {
        visitor_p->visit(this);
    }
};

/// @brief this is a place holder step to use a key for step handling
class CustomKeyStep : public CustomStep
{
public:
    CustomKeyStep(std::string const &key_p) : _key(key_p) {}
    /// @brief apply this Steppable to the given state
    virtual void apply(State &state_p, SteppableData *) const {}
    /// @brief revert this Steppable to the given state
    virtual void revert(State &state_p, SteppableData *) const {}

    /// @brief return true if this Steppable does no operation on the State it would be applied
    virtual bool isNoOp() const { return false; }

    std::string const _key;
};

}

#endif
