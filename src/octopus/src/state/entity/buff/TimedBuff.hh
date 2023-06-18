#ifndef __TimedBuff__
#define __TimedBuff__

#include <string>
#include "Buff.hh"
#include "utils/Fixed.hh"

namespace octopus
{

struct TimedBuff : public TyppedBuff
{
    /// @brief 0 for infinite buff
	unsigned long _duration {0};

	/// @brief true if applies to enemies
	bool _debuff {false};

	std::string _id;

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

struct Buffer
{
	/// @brief true if the buffer is active (false if no buff to apply)
	bool _active {false};
	/// @brief reload time to apply buff
	unsigned long _reload {0};
	/// @brief range to apply buff
	Fixed _range {3.};
	/// @brief the buff than can be applied
	TimedBuff _buff;
};

} // namespace octopus

#endif
