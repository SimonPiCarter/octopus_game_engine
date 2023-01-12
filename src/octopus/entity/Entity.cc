#include "Entity.hh"

std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<"Entity[pos="<<ent_p._pos<<", ray="<<ent_p._ray<<", frozen="<<ent_p._frozen<<"]";
}

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<ent_p._pos.x<<","<<ent_p._pos.y<<","<<ent_p._ray;
}

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity> const &vec_p)
{
	os_p<<"x,y,r"<<std::endl;
	for(octopus::Entity const &ent_l : vec_p)
	{
		streamCsvPos(os_p, ent_l) << std::endl;
	}
	return os_p;
}
