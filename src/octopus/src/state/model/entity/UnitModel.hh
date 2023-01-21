#ifndef __UnitModel__
#define __UnitModel__

#include <map>

#include "state/ResourceType.hh"
#include "EntityModel.hh"

namespace octopus
{
	struct UnitModel : public EntityModel
	{
		UnitModel(bool isStatic_p, double ray_p, double stepSpeed_p, double hpMax_p)
			: EntityModel(isStatic_p, ray_p, stepSpeed_p, hpMax_p)
		{}

		std::map<ResourceType, double> _maxQuantity;

		/// @brief production time in steps
		unsigned long _productionTime;
	};
} // namespace octopus


#endif
