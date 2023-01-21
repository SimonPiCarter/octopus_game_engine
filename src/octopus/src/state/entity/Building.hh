#ifndef __Building__
#define __Building__

#include "Entity.hh"
#include "utils/Vector.hh"

namespace octopus
{

class Building : public Entity
{
	public:
		Building(Vector const &pos_p, bool frozen_p, EntityModel const &model_p);

		Vector _productionOutput;
};

}

#endif
