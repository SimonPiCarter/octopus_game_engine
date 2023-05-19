#ifndef __DamageOverTime__
#define __DamageOverTime__

#include "command/flying/CommandEffectOverTime.hh"

namespace octopus
{

/// @brief This class is a CommandEffectOverTime
/// aimed at reducing a resource, 1 resource per second
/// if resource if at 0 then deal 10 damage per second
class DamageOverTime : public CommandEffectOverTime
{
public:
    DamageOverTime(Handle handle_p, unsigned long tickRate_p, unsigned long nbOfTicks_p, Fixed dmg_p, Handle ent_p)
        : CommandEffectOverTime(handle_p, tickRate_p, nbOfTicks_p), _dmg(dmg_p), _ent(ent_p) {}

	virtual void applyEffect(Step & step_p, State const &state_p, CommandData const * , PathManager &) const override;
protected:
    /// @brief damage per tick
    Fixed const _dmg;
    /// @brief the handle of entity to deal damage to
    Handle const _ent;
};

}

#endif
