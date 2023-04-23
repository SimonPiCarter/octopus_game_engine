#ifndef __Building__
#define __Building__

#include "Entity.hh"
#include "utils/Vector.hh"
#include "state/model/entity/BuildingModel.hh"

namespace octopus
{

class Building : public Entity
{
	public:
		Building(Vector const &pos_p, bool frozen_p, BuildingModel const &model_p);

		/// @brief building progress in steps
		double _buildingProgress {0};

		BuildingModel const &_buildingModel;

		/// @brief true if the building has been canceled
		/// this will happen when building was not even started but
		/// the space was taken (or if cancelled manually)
		bool _canceled {false};

		///
		/// Rally point
		///

		/// @brief true if there is an active rally point
		bool _rallyPointActive {false};
		/// @brief position of the rally point
		Vector _rallyPoint;
		/// @brief true if there is an active rally point
		bool _rallyPointEntityActive {false};
		/// @brief entity pointed by the rally point
		Handle _rallyPointEntity;

		/// @brief return true if building is done building
		bool isBuilt() const;
		/// @brief return true if building is still a blueprint
		/// has not started
		bool isBlueprint() const;
};

}

#endif
