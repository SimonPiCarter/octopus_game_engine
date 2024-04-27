
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
		case TyppedBuff::Type::Heal:
			return "Heal";
		case TyppedBuff::Type::Armor:
			return "Armor";
		case TyppedBuff::Type::HpMax:
			return "HpMax";
		case TyppedBuff::Type::HpRegeneration:
			return "HpRegeneration";
		case TyppedBuff::Type::Production:
			return "Production";
		case TyppedBuff::Type::Harvest:
			return "Harvest";
		case TyppedBuff::Type::DamageReturn:
			return "DamageReturn";
	}
	throw std::logic_error("missing to string implem");
}

} // namespace octopus
