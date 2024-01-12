#ifndef __UnitModel__
#define __UnitModel__

#include "EntityModel.hh"
#include "state/entity/buff/Buffer.hh"

namespace octopus
{

	class StepShallow;
	class Command;

	/// @brief create command when idle or attack moving
	/// @param waitingTimeForAttackScan_p number of waiting step before looking for a target to attack (should be 0 when auto attacking)
	Command * commandFromIdle(Entity const &ent_p, State const &state_p, unsigned long waitingTimeForAttackScan_p);

	void unitIdleFunction(Entity const &ent_p, StepShallow & step_p, State const &state_p);

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
