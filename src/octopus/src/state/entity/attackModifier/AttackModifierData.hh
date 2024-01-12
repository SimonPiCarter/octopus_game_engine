#ifndef __AttackModifierData__
#define __AttackModifierData__

#include "state/Handle.hh"
#include "utils/Fixed.hh"
#include "utils/Vector.hh"

namespace octopus
{

class State;
class StepShallow;

/// @brief the data required to apply a modifier
struct AttackModifierData
{
	Handle const source;
	Handle const target;
	Vector const targetPosition;
	unsigned long const team;
	/// @brief damage with NO bonuses
	Fixed const baseDamage;
	/// @brief total damage with bonuses
	Fixed const bonusDamage;
};

/// @brief apply main attack damage (no modifier)
void applyMainAttack(StepShallow &step_p, AttackModifierData const &data_p, State const &state_p);

} // octopus
#endif