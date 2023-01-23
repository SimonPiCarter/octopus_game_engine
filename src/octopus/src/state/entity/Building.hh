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
		unsigned long _buildingProgress {0};

		BuildingModel const &_buildingModel;
};

}

#endif
