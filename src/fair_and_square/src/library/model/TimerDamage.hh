#ifndef __TimerDamage__
#define __TimerDamage__

#include "command/flying/CommandEffectOverTime.hh"
#include "state/ResourceType.hh"

/// @brief This class is a CommandEffectOverTime
/// aimed at reducing a resource, 1 resource per second
/// if resource if at 0 then deal 10 damage per second
class TimerDamage : public octopus::CommandEffectOverTime
{
public:
    TimerDamage(octopus::Handle handle_p, unsigned long tickRate_p, unsigned long nbOfTicks_p,
        unsigned long player_p, octopus::ResourceType res_p, octopus::Handle ent_p)
        : CommandEffectOverTime(handle_p, tickRate_p, nbOfTicks_p), _player(player_p), _res(res_p) , _ent(ent_p) {}

	/// @brief virtual pure method to apply the effect
	/// @note add steppables here
	virtual void applyEffect(octopus::Step & step_p, octopus::State const &state_p, octopus::CommandData const * , octopus::PathManager &) const override;
protected:
    /// @brief the handle of entity to deal damage to
    unsigned long const _player;
    /// @brief the resource to decrease and check
    octopus::ResourceType const _res;
    /// @brief the handle of entity to deal damage to
    octopus::Handle const _ent;
};

#endif