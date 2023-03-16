#ifndef __Buff__
#define __Buff__

#include <string>

namespace octopus
{
class Entity;
class State;

struct Buff
{
	double _offset {0.};
	double _coef {0.};
};

struct TyppedBuff : public Buff
{
	enum class Type
	{
		Speed,
		FullReload,
		Damage,
		Armor,
		HpMax,
		Production,
		Harvest
	};

	Type _type {Type::Speed};

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
	double _range {3.};
	/// @brief the buff than can be applied
	TyppedBuff _buff;
};

}

#endif
