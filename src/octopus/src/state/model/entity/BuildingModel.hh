#ifndef __BuildingModel__
#define __BuildingModel__

#include <list>

#include "utils/Vector.hh"
#include "EntityModel.hh"

namespace octopus
{
	class Building;
	class State;
	struct UnitModel;;

	struct BuildingModel : public EntityModel
	{
		BuildingModel(bool isStatic_p, Fixed ray_p, Fixed hpMax_p);

		std::map<ResourceType, bool> _deposit;

		/// @brief building time in steps
		unsigned long _buildingTime {0};

		/// @brief relative position for output production
		Vector _productionOutput { 1, 0 };

		/// @brief list of UnitModel available in this building
		std::list<UnitModel const *> _unitModels;

		/// @brief safe getter for the deposit map
		bool isDeposit(ResourceType type_p) const;

		bool isAnyDeposit() const;

		bool isProduction() const;

		bool canProduce(UnitModel const * model_p) const;

		virtual bool checkLegality(Building const &, State const &) const;

		/// @brief responsible for checking the grid in state
		virtual bool checkGrid(Building const &building_p, State const &state_p) const;
	};
} // namespace octopus


#endif
