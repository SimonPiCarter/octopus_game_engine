#ifndef __BuildingModel__
#define __BuildingModel__

#include <map>

#include "state/ResourceType.hh"
#include "utils/Vector.hh"
#include "EntityModel.hh"

namespace octopus
{
	struct BuildingModel : public EntityModel
	{
		BuildingModel(bool isStatic_p, double ray_p, double hpMax_p)
			: EntityModel(isStatic_p, ray_p, 0., hpMax_p)
		{}

		std::map<ResourceType, bool> _deposit;

		std::map<ResourceType, double> _cost;

		/// @brief building time in steps
		unsigned long _buildingTime {0};

		/// @brief relative position for output production
		Vector _productionOutput;
	};
} // namespace octopus


#endif
