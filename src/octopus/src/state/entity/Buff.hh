#ifndef __Buff__
#define __Buff__

#include <string>

namespace octopus
{
class Entity;

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
		Production,
		Harvest
	};

	Type _type {Type::Speed};

	unsigned long _duration {0};

	std::string _id;

	void apply(Entity &ent_p) const;
	void revert(Entity &ent_p) const;
};

}

#endif
