#ifndef __CompositeBuff__
#define __CompositeBuff__

#include <string>
#include <variant>
#include "BaseBuff.hh"
#include "TimedBuff.hh"
#include "ModifierAoEBuff.hh"
#include "ModifierBuff.hh"
#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

namespace octopus
{

using CompositableBuff = std::variant<TimedBuff, ModifierBuff, ModifierAoEBuff>;

struct CompositeBuff : public BaseBuff
{
	std::vector<CompositableBuff> _buffs;

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
