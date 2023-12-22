#ifndef __TimedBuff__
#define __TimedBuff__

#include <optional>
#include <string>
#include "Buff.hh"
#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

namespace octopus
{

struct TimedBuff : public TyppedBuff
{
    /// @brief 0 for infinite buff
	unsigned long _duration {0};

	/// @brief true if applies to enemies
	bool _debuff {false};

	std::string _id;

	/// @brief optional attack modifier
	std::optional<AttackModifier> _attackMod = std::nullopt;

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
