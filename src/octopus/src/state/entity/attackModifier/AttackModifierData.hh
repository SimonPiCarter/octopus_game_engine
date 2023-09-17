#ifndef __AttackModifierData__
#define __AttackModifierData__

#include "state/Handle.hh"
#include "utils/Fixed.hh"

namespace octopus
{

class State;
class Step;

/// @brief the data required to apply a modifier
struct AttackModifierData
{
	Handle const source;
	Handle const target;
	unsigned long const player;
	unsigned long const team;
	/// @brief damage with NO bonuses
	Fixed baseDamage;
	/// @brief total damage with bonuses
	Fixed bonusDamage;
};

/// @brief apply main attack damage (no modifier)
void applyMainAttack(Step &step_p, AttackModifierData const &data_p, State const &state_p);

} // octopus
#endif