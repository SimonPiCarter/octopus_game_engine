#ifndef __UnitModel__
#define __UnitModel__

#include "EntityModel.hh"
#include "state/entity/buff/Buffer.hh"

namespace octopus
{
	class Command;
	/// @brief create command when idle or attack moving
	/// @param waitingTimeForAttackScan_p number of waiting step before looking for a target to attack (should be 0 when auto attacking)
	Command * commandFromIdle(Entity const &ent_p, State const &state_p, unsigned long waitingTimeForAttackScan_p);

	void unitIdleFunction(Entity const &ent_p, Step & step_p, State const &state_p);


	/// @brief can be extended but must be clonable and copyable
	struct UnitData
	{
		/// @brief Create a new UnitData that is a copy of this one
		/// @note a new allocation must be performed
		virtual UnitData* clone() const = 0;
		/// @brief copy everything from another UnitData
		virtual void copyFrom(UnitData const *other_p) = 0;
	};
	/// @brief can be extended
	struct StaticUnitData {};

	struct UnitModel : public EntityModel
	{
		UnitModel(bool isStatic_p, Fixed ray_p, Fixed stepSpeed_p, Fixed hpMax_p);
		~UnitModel() { delete _unitData; delete _staticUnitData; }

		std::map<std::string, Fixed> _maxQuantity;

		/// @brief production time in steps
		unsigned long _productionTime {0};

		/// @brief step time required to harvest 1 resource
		unsigned long _timeToHarvest {1};

		/// @brief the buffer info of this unit
		Buffer _buffer;

		/// @brief used to store specific unit data that might be modified
		/// a copy will be used as a model for every unit
		UnitData * _unitData = nullptr;
		/// @brief used to store specific unit data that will be static
		/// The same pointer will be used for all units
		StaticUnitData * _staticUnitData = nullptr;
	};
} // namespace octopus


#endif
