#ifndef __BuildingModel__
#define __BuildingModel__

#include "state/ResourceType.hh"
#include "state/model/entity/EntityModel.hh"

namespace octopus
{
	struct BuildingModel : public EntityModel
	{
		BuildingModel(bool isStatic_p, double ray_p, double stepSpeed_p, double hpMax_p)
			: EntityModel(isStatic_p, ray_p, stepSpeed_p, hpMax_p)
		{}

		std::map<ResourceType, bool> _deposit;
	};
} // namespace octopus


#endif
