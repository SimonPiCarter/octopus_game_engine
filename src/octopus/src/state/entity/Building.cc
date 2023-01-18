#include "Building.hh"

namespace octopus
{

Building::Building(Vector const &pos_p, bool frozen_p, EntityModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
{}

} // namespace octopus
