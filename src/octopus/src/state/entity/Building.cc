#include "Building.hh"

namespace octopus
{

Building::Building(Vector const &pos_p, bool frozen_p, BuildingModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
	, _buildingModel(model_p)
{}

bool Building::isBuilt() const
{
	return _buildingProgress >= _buildingModel._buildingTime;
}
bool Building::isBlueprint() const
{
	return _buildingProgress <= 1e-5;
}
} // namespace octopus
