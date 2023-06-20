#include "Unit.hh"

namespace octopus
{

Unit::Unit(Vector const &pos_p, bool frozen_p, UnitModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
	, _unitModel(model_p)
{}

} // namespace octopus
