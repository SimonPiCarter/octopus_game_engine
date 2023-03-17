
#include "Buff.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace octopus
{

bool TyppedBuff::isApplying(State const &state_p, Entity const &ent_p) const
{
	if(_type == Type::Speed
	|| _type == Type::FullReload
	|| _type == Type::Damage
	|| _type == Type::Armor
	|| _type == Type::HpMax)
	{
		return ent_p._model._isUnit;
	}
	if(_type == Type::Production)
	{
		return ent_p._model._isBuilding && static_cast<Building const &>(ent_p)._buildingModel.isProduction();
	}
	if(_type == Type::Harvest)
	{
		return ent_p._model._isBuilding && static_cast<Building const &>(ent_p)._buildingModel.isAnyDeposit();
	}
	return false;
}

bool TyppedBuff::isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const
{
	unsigned long teamSource_l = state_p.getPlayer(source_p._player)->_team;
	unsigned long teamEnt_l = state_p.getPlayer(ent_p._player)->_team;
	// cannot buff enemy
	if(!_debuff && teamSource_l != teamEnt_l)
	{
		return false;
	}
	// cannot debuff allies
	if(_debuff && teamSource_l == teamEnt_l)
	{
		return false;
	}
	return isApplying(state_p, ent_p);
}

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
		case Type::HpMax:
			ent_p._buffHpMax._offset += _offset;
			ent_p._buffHpMax._coef += _coef;
			break;
		case Type::Production:
			ent_p._buffProduction._offset += _offset;
			ent_p._buffProduction._coef += _coef;
			break;
		case Type::Harvest:
			ent_p._buffHarvest._offset += _offset;
			ent_p._buffHarvest._coef += _coef;
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
		case Type::HpMax:
			ent_p._buffHpMax._offset -= _offset;
			ent_p._buffHpMax._coef -= _coef;
			break;
		case Type::Production:
			ent_p._buffProduction._offset -= _offset;
			ent_p._buffProduction._coef -= _coef;
			break;
		case Type::Harvest:
			ent_p._buffHarvest._offset -= _offset;
			ent_p._buffHarvest._coef -= _coef;
			break;
	}
}

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
