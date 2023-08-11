#ifndef __RESOURCE__
#define __RESOURCE__

#include "Entity.hh"
#include "state/model/entity/ResourceModel.hh"

namespace octopus
{

class Resource : public Entity
{
	public:
		Resource(Vector const &pos_p, bool frozen_p, ResourceModel const &model_p);

		Fixed _resource {0.};

		ResourceModel const &_resourceModel;

		std::string const &getType() const { return _resourceModel._type; }
};

}

#endif
