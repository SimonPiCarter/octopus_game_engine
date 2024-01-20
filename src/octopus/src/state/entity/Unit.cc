#include "Unit.hh"

namespace octopus
{

Unit::Unit(Vector const &pos_p, bool frozen_p, UnitModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
	, _unitModel(model_p)
	, _unitData(_unitModel._unitData)
	, _staticUnitData(model_p._staticUnitData.get())
{}

} // namespace octopus
