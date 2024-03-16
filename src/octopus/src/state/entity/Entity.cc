#include "Entity.hh"

#include <limits>

#include "command/CommandData.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

namespace octopus
{

Entity::Entity(Vector const &pos_p, bool frozen_p, EntityModel const &model_p)
	: Commandable()
	, _pos(pos_p)
	, _frozen(frozen_p)
	, _hp(model_p._hpMax)
	, _reload(model_p._fullReload)
	, _model(model_p)
	, _attackMod(model_p._defaultAttackMod)
{}

bool Entity::isIgnoringCollision() const
{
	if(getQueue().hasCommand())
	{
		return getData(getQueue().getFrontCommand()._var)
			&& getData(getQueue().getFrontCommand()._var)->_ignoreCollision;
	}
	return false;
}

Fixed applyBuff(Entity const &ent_p, Fixed val_p, Buff const &buff_p, std::vector<ConditionalBuff> const &condBuffs_p)
{
	Fixed offsetVal_l = val_p + buff_p._offset;
	Fixed coefVal_l = Fixed::One() + buff_p._coef;
	for(ConditionalBuff const &buff_l : condBuffs_p)
	{
		if(buff_l.isApplying(ent_p))
		{
			offsetVal_l += buff_l._offset;
			coefVal_l += buff_l._coef;
		}
	}
	offsetVal_l = std::max(Fixed(0.), offsetVal_l);
	coefVal_l = std::max( Fixed(0.1), coefVal_l );
	return offsetVal_l * coefVal_l;
}

Fixed Entity::getStepSpeed() const
{
	return applyBuff( *this, _model._stepSpeed, _buffSpeed, _condBuffSpeed);
}

Fixed Entity::getFullReload() const
{
	return applyBuff( *this, _model._fullReload, _buffFullReload, _condBuffFullReload);
}

Fixed Entity::getDamage(EntityModel const &target_p) const
{
	return applyBuff( *this, _model._damage, _buffDamage, _condBuffDamage) + getBonus(target_p._id, _model);
}
Fixed Entity::getDamageNoBonus() const
{
	return applyBuff( *this, _model._damage, _buffDamage, _condBuffDamage);
}
Fixed Entity::getHeal() const
{
	return applyBuff( *this, _model._heal, _buffHeal, _condBuffHeal);
}

Fixed Entity::getArmor() const
{
	return applyBuff( *this, _model._armor, _buffArmor, _condBuffArmor);
}
Fixed Entity::getHpMax(bool applyConditional_p) const
{
	return std::max(Fixed(1), applyBuff( *this, _model._hpMax, _buffHpMax, applyConditional_p?_condBuffHpMax:std::vector<ConditionalBuff>()));
}

Fixed Entity::getHpRegeneration() const
{
	return std::max(Fixed(0), applyBuff( *this, _model._hpRegeneration, _buffHpRegeneration, _condBuffHpRegeneration));
}

Fixed Entity::getProduction() const
{
	return applyBuff( *this, Fixed::One(), _buffProduction, _condBuffProduction);
}

Fixed Entity::getHarvest() const
{
	return applyBuff( *this, Fixed::One(), _buffHarvest, _condBuffHarvest);
}

Fixed Entity::getDamageReturn() const
{
	return std::max(Fixed(0), applyBuff( *this, _model._damageReturn, _buffDamageReturn, _condBuffDamageReturn));
}

bool Entity::isFrozen() const
{
	return _frozen || _model._isStatic;
}

unsigned long Entity::getTimeSinceBuff(std::string const &idBuff_p) const
{
	auto &&it_l = _timeSinceBuff.find(idBuff_p);
	if(it_l == _timeSinceBuff.end())
	{
		return std::numeric_limits<unsigned long>::max();
	}
	return it_l->second;
}

void Entity::runCommands(Step & step_p, State const &state_p, PathManager &pathManager_p)
{
	if(_waiting < 100000)
	{
		step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, _waiting+1));
	}
	if(!getQueue().hasCommand())
	{
		_model._idleFunc(*this, step_p, state_p);
	}
	Commandable::runCommands(step_p, state_p, pathManager_p);
}

unsigned long getReloadAbilityTime(Entity const &ent_p, std::string const &key_p, unsigned long const &default_p)
{
	auto &&it_l = ent_p._reloadAbilities.find(key_p);
	if(it_l == ent_p._reloadAbilities.end())
	{
		return default_p;
	}
	return it_l->second;
}

} // namespace octopus


std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<"Entity[pos="<<ent_p._pos<<", ray="<<ent_p._model._ray<<", frozen="<<ent_p._frozen<<"]";
}

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<ent_p._pos.x<<","<<ent_p._pos.y<<","<<ent_p._model._ray<<","<<ent_p._hp<<","<<ent_p._model._isStatic;
}

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity *> const &vec_p)
{
	os_p<<"x,y,r,hp,building"<<std::endl;
	for(octopus::Entity const *ent_l : vec_p)
	{
		if(ent_l->_alive)
		{
			streamCsvPos(os_p, *ent_l) << std::endl;
		}
	}
	return os_p;
}
