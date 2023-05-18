#ifndef __RESOURCE__
#define __RESOURCE__

#include "Entity.hh"
#include "state/ResourceType.hh"

namespace octopus
{

class Resource : public Entity
{
	public:
		Resource(Vector const &pos_p, bool frozen_p, EntityModel const &model_p);

		ResourceType _type { ResourceType::Food };
		Fixed _resource {0.};
};

}

#endif
