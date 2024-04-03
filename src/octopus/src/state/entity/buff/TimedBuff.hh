#ifndef __TimedBuff__
#define __TimedBuff__

#include <optional>
#include <string>
#include "BaseBuff.hh"
#include "Buff.hh"
#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

namespace octopus
{

struct TimedBuff : public TyppedBuff, BaseBuff
{
	TimedBuff() {}
	TimedBuff(Fixed const &offset_p, Fixed const &coef_p, Type type_p, unsigned long duration_p, bool debuff_p, std::string const &id_p);
	/// @brief true if applies to enemies
	bool _debuff {false};

	/// @brief check if the buff apply to the entity
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	bool isApplying(State const &state_p, Entity const &ent_p) const override;

	/// @brief check if the buff apply to the entity
	/// @param source_p the entity buffing
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	bool isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const override;

	void apply(Entity &ent_p) const override;
	void revert(Entity &ent_p) const override;
};

} // namespace octopus

#endif
