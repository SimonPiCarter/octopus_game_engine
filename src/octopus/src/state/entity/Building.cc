#include "Building.hh"

namespace octopus
{

Building::Building(Vector const &pos_p, bool frozen_p, EntityModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
	, _productionOutput(pos_p + Vector(model_p._ray, 0))
{}

} // namespace octopus
