#ifndef __ModifierAoEBuff__
#define __ModifierAoEBuff__

#include <string>
#include "BaseBuff.hh"
#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

namespace octopus
{

struct ModifierAoEBuff : public BaseBuff
{
	Fixed _deltaRatio;
	Fixed _deltaRange;

	/// @brief check if the buff apply to the entity
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	bool isApplying(State const &state_p, Entity const &ent_p) const;

	/// @brief check if the buff apply to the entity
	/// @param source_p the entity buffing
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	bool isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const;

	void apply(Entity &ent_p) const;
	void revert(Entity &ent_p) const;
};

} // namespace octopus

#endif
