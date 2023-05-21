#ifndef __RESOURCE__
#define __RESOURCE__

#include "Entity.hh"

namespace octopus
{

class Resource : public Entity
{
	public:
		Resource(Vector const &pos_p, bool frozen_p, EntityModel const &model_p);

		std::string _type { "bloc" };
		Fixed _resource {0.};
};

}

#endif
