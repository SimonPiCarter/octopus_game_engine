
#include "TimedBuff.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace octopus
{

bool TimedBuff::isApplying(State const &state_p, Entity const &ent_p) const
{
	if(_attackMod)
	{
		return ent_p._model._isUnit && std::holds_alternative<NoModifier>(ent_p._attackMod);
	}
	if(_type == Type::Speed
	|| _type == Type::FullReload
	|| _type == Type::Damage
	|| _type == Type::Heal
	|| _type == Type::Armor
	|| _type == Type::HpMax
	|| _type == Type::HpRegeneration)
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

bool TimedBuff::isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const
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

void TimedBuff::apply(Entity &ent_p) const
{
	if(_attackMod && std::holds_alternative<NoModifier>(ent_p._attackMod))
	{
		ent_p._attackMod = *_attackMod;
		return;
	}
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
		case Type::Heal:
			ent_p._buffHeal._offset += _offset;
			ent_p._buffHeal._coef += _coef;
			break;
		case Type::Armor:
			ent_p._buffArmor._offset += _offset;
			ent_p._buffArmor._coef += _coef;
			break;
		case Type::HpMax:
		{
			Fixed hpMax_l = ent_p.getHpMax();
			ent_p._buffHpMax._offset += _offset;
			ent_p._buffHpMax._coef += _coef;
			Fixed newHpMax_l = ent_p.getHpMax();
			ent_p._hp += newHpMax_l - hpMax_l;
			break;
		}
		case Type::HpRegeneration:
			ent_p._buffHpRegeneration._offset += _offset;
			ent_p._buffHpRegeneration._coef += _coef;
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

void TimedBuff::revert(Entity &ent_p) const
{
	if(_attackMod)
	{
		ent_p._attackMod = NoModifier();
		return;
	}
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
		case Type::Heal:
			ent_p._buffHeal._offset -= _offset;
			ent_p._buffHeal._coef -= _coef;
			break;
		case Type::Armor:
			ent_p._buffArmor._offset -= _offset;
			ent_p._buffArmor._coef -= _coef;
			break;
		case Type::HpMax:
		{
			Fixed hpMax_l = ent_p.getHpMax();
			ent_p._buffHpMax._offset -= _offset;
			ent_p._buffHpMax._coef -= _coef;
			Fixed newHpMax_l = ent_p.getHpMax();
			ent_p._hp += newHpMax_l - hpMax_l;
			break;
		}
		case Type::HpRegeneration:
			ent_p._buffHpRegeneration._offset -= _offset;
			ent_p._buffHpRegeneration._coef -= _coef;
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


} // namespace octopus
