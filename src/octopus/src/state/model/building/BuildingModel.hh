#ifndef __BuildingModel__
#define __BuildingModel__

#include "state/ResourceType.hh"
#include "state/model/entity/EntityModel.hh"

namespace octopus
{
	struct BuildingModel : public EntityModel
	{
		std::map<ResourceType, bool> _deposit;
	};
} // namespace octopus


#endif
