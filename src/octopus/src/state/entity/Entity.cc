#include "Entity.hh"

namespace octopus
{

Entity::Entity(Vector const &pos_p, bool frozen_p, EntityModel const &model_p)
	: _pos(pos_p)
	, _frozen(frozen_p)
	, _hp(model_p._hpMax)
	, _model(model_p)
{}

} // namespace octopus


std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<"Entity[pos="<<ent_p._pos<<", ray="<<ent_p._model._ray<<", frozen="<<ent_p._frozen<<"]";
}

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<ent_p._pos.x<<","<<ent_p._pos.y<<","<<ent_p._model._ray<<","<<ent_p._hp<<","<<ent_p._model._isBuilding;
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
