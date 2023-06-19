
#include "Buff.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace octopus
{

std::string to_string(TyppedBuff::Type type_p)
{
	switch(type_p)
	{
		case TyppedBuff::Type::Speed:
			return "Speed";
		case TyppedBuff::Type::FullReload:
			return "FullReload";
		case TyppedBuff::Type::Damage:
			return "Damage";
		case TyppedBuff::Type::Armor:
			return "Armor";
		case TyppedBuff::Type::HpMax:
			return "HpMax";
		case TyppedBuff::Type::Production:
			return "Production";
		case TyppedBuff::Type::Harvest:
			return "Harvest";
	}
	throw std::logic_error("missing to string implem");
}

} // namespace octopus