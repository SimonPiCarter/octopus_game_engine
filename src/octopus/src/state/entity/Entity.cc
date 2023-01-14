#include "Entity.hh"

namespace octopus
{

Entity::Entity(Vector const &pos_p, double ray_p, bool frozen_p, double stepSpeed_p)
	: _pos(pos_p)
	, _ray(ray_p)
	, _frozen(frozen_p)
	, _stepSpeed(stepSpeed_p)
{}

} // namespace octopus


std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<"Entity[pos="<<ent_p._pos<<", ray="<<ent_p._ray<<", frozen="<<ent_p._frozen<<"]";
}

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<ent_p._pos.x<<","<<ent_p._pos.y<<","<<ent_p._ray<<","<<ent_p._stats._hp;
}

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity *> const &vec_p)
{
	os_p<<"x,y,r,hp"<<std::endl;
	for(octopus::Entity const *ent_l : vec_p)
	{
		streamCsvPos(os_p, *ent_l) << std::endl;
	}
	return os_p;
}
