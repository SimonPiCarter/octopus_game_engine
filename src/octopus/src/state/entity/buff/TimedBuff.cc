
#include "TimedBuff.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace octopus
{

TimedBuff::TimedBuff(Fixed const &offset_p, Fixed const &coef_p, Type type_p, unsigned long duration_p, bool debuff_p, std::string const &id_p)
{
	_offset = offset_p;
	_coef = coef_p;
	_type = type_p;
	_duration = duration_p;
	_debuff = debuff_p;
	_id = id_p;
}

bool TimedBuff::isApplying(State const &state_p, Entity const &ent_p) const
{
	if(_type == Type::Speed
	|| _type == Type::FullReload
	|| _type == Type::Damage
	|| _type == Type::Heal
	|| _type == Type::Armor
	|| _type == Type::HpMax
	|| _type == Type::HpRegeneration
	|| _type == Type::DamageReturn)
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
		case Type::DamageReturn:
			ent_p._buffDamageReturn._offset += _offset;
			ent_p._buffDamageReturn._coef += _coef;
			break;
	}
}

void TimedBuff::revert(Entity &ent_p) const
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
		case Type::DamageReturn:
			ent_p._buffDamageReturn._offset -= _offset;
			ent_p._buffDamageReturn._coef -= _coef;
			break;
	}
}


} // namespace octopus
