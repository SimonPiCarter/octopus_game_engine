#include "Entity.hh"

#include <limits>

#include "command/CommandData.hh"

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

Fixed applyBuff(Fixed val_p, Buff const &buff_p)
{
	return std::max(Fixed(0.), val_p + buff_p._offset ) * std::max( Fixed(0.1), 1. + buff_p._coef );
}

Fixed Entity::getStepSpeed() const
{
	return applyBuff( _model._stepSpeed, _buffSpeed);
}

Fixed Entity::getFullReload() const
{
	return applyBuff( _model._fullReload, _buffFullReload);
}

Fixed Entity::getDamage(EntityModel const &target_p) const
{
	return applyBuff( _model._damage, _buffDamage) + getBonus(target_p._id, _model);
}
Fixed Entity::getDamageNoBonus() const
{
	return applyBuff( _model._damage, _buffDamage);
}
Fixed Entity::getArmor() const
{
	return applyBuff( _model._armor, _buffArmor);
}
Fixed Entity::getHpMax() const
{
	return std::max(Fixed(1), applyBuff( _model._hpMax, _buffHpMax));
}

Fixed Entity::getProduction() const
{
	return applyBuff( 1., _buffProduction);
}

Fixed Entity::getHarvest() const
{
	return applyBuff( 1., _buffHarvest);
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
