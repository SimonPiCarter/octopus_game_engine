#ifndef __UnitModel__
#define __UnitModel__

#include "EntityModel.hh"
#include "state/entity/Buff.hh"

namespace octopus
{
	struct UnitModel : public EntityModel
	{
		UnitModel(bool isStatic_p, double ray_p, double stepSpeed_p, double hpMax_p)
			: EntityModel(isStatic_p, ray_p, stepSpeed_p, hpMax_p)
		{
			_isUnit = true;
		}

		std::map<ResourceType, double> _maxQuantity;

		/// @brief production time in steps
		unsigned long _productionTime {0};

		/// @brief step time required to harvest 1 resource
		unsigned long _timeToHarvest {1};

		/// @brief the buffer info of this unit
		Buffer _buffer;
	};
} // namespace octopus


#endif
