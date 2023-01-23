#include "Building.hh"

namespace octopus
{

Building::Building(Vector const &pos_p, bool frozen_p, BuildingModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
	, _buildingModel(model_p)
{}

} // namespace octopus
