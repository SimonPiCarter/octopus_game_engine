#ifndef __Building__
#define __Building__

#include "Entity.hh"

namespace octopus
{

class Building : public Entity
{
	public:
		Building(Vector const &pos_p, bool frozen_p, EntityModel const &model_p);
};

}

#endif
