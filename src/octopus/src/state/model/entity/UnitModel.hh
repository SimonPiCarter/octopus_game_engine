#ifndef __UnitModel__
#define __UnitModel__

#include "EntityModel.hh"
#include "state/entity/buff/Buffer.hh"

namespace octopus
{
	void unitIdleFunction(Entity const &ent_p, Step & step_p, State const &state_p);

	struct UnitModel : public EntityModel
	{
		UnitModel(bool isStatic_p, Fixed ray_p, Fixed stepSpeed_p, Fixed hpMax_p);

		std::map<std::string, Fixed> _maxQuantity;

		/// @brief production time in steps
		unsigned long _productionTime {0};

		/// @brief step time required to harvest 1 resource
		unsigned long _timeToHarvest {1};

		/// @brief the buffer info of this unit
		Buffer _buffer;
	};
} // namespace octopus


#endif
