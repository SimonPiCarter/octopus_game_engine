#ifndef __TempleModel__
#define __TempleModel__

#include "BuildingModel.hh"

namespace octopus
{
	struct TempleModel : public BuildingModel
	{
		TempleModel(bool isStatic_p, Fixed ray_p, Fixed hpMax_p)
			: BuildingModel(isStatic_p, ray_p, hpMax_p)
		{
			_isBuilding = true;
			_stepSpeed = 0.;
			_damage = 0.;
		}

		/// @brief Check if the temple match one of the temple position in the state
		/// @param building_p the building to be tested
		/// @param state_p the state to be tested
		/// @return true if the temple can be placed
		virtual bool checkLegality(Building const &building_p, State const &state_p) const override;

		/// @brief responsible for checking the grid in state
		virtual bool checkGrid(Building const &building_p, State const &state_p) const;
	};
} // namespace octopus


#endif
