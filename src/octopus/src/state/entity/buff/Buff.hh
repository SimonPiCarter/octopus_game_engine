#ifndef __Buff__
#define __Buff__

#include <string>
#include "utils/Fixed.hh"

namespace octopus
{
class Entity;
class State;

struct Buff
{
	Fixed _offset {0.};
	Fixed _coef {0.};
};

struct TyppedBuff : public Buff
{
	enum class Type
	{
		Speed,
		FullReload,
		Damage,
		Heal,
		Armor,
		HpMax,
		HpRegeneration,
		Production,
		Harvest
	};

	Type _type {Type::Speed};
};

std::string to_string(TyppedBuff::Type type_p);

}

#endif
