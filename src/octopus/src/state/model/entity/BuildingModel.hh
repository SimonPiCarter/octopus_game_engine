#ifndef __BuildingModel__
#define __BuildingModel__

#include <list>

#include "utils/Vector.hh"
#include "EntityModel.hh"

namespace octopus
{
	class UnitModel;

	struct BuildingModel : public EntityModel
	{
		BuildingModel(bool isStatic_p, double ray_p, double hpMax_p)
			: EntityModel(isStatic_p, ray_p, 0., hpMax_p)
		{}

		std::map<ResourceType, bool> _deposit;

		/// @brief building time in steps
		unsigned long _buildingTime {0};

		/// @brief relative position for output production
		Vector _productionOutput { 1, 0 };

		/// @brief list of UnitModel available in this building
		std::list<UnitModel const *> _unitModels;
	};
} // namespace octopus


#endif