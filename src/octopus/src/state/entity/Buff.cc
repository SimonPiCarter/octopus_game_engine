
#include "Buff.hh"
#include "Entity.hh"

namespace octopus
{

void TyppedBuff::apply(Entity &ent_p) const
{
	switch(_type)
	{
		case Type::Speed:
			ent_p._buffSpeed._offset += _offset;
			ent_p._buffSpeed._coef += _coef;
			break;
		case Type::FullReload:
			ent_p._buffFullReload._offset += _offset;
			ent_p._buffFullReload._coef += _coef;
			break;
		case Type::Damage:
			ent_p._buffDamage._offset += _offset;
			ent_p._buffDamage._coef += _coef;
			break;
		case Type::Armor:
			ent_p._buffArmor._offset += _offset;
			ent_p._buffArmor._coef += _coef;
			break;
	}
}

void TyppedBuff::revert(Entity &ent_p) const
{
	switch(_type)
	{
		case Type::Speed:
			ent_p._buffSpeed._offset -= _offset;
			ent_p._buffSpeed._coef -= _coef;
			break;
		case Type::FullReload:
			ent_p._buffFullReload._offset -= _offset;
			ent_p._buffFullReload._coef -= _coef;
			break;
		case Type::Damage:
			ent_p._buffDamage._offset -= _offset;
			ent_p._buffDamage._coef -= _coef;
			break;
		case Type::Armor:
			ent_p._buffArmor._offset -= _offset;
			ent_p._buffArmor._coef -= _coef;
			break;
	}
}

} // namespace octopus
