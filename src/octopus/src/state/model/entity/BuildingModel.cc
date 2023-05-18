#include "BuildingModel.hh"

#include <list>

#include "utils/Vector.hh"
#include "EntityModel.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"

namespace octopus
{

BuildingModel::BuildingModel(bool isStatic_p, Fixed ray_p, Fixed hpMax_p)
	: EntityModel(isStatic_p, ray_p, 0., hpMax_p)
{
	_isBuilding = true;
	_stepSpeed = 0.;
	_damage = 0.;
}

bool BuildingModel::isDeposit(ResourceType type_p) const
{
	auto && it_l = _deposit.find(type_p);
	if(it_l != _deposit.end())
	{
		return it_l->second;
	}
	return false;
}

bool BuildingModel::isAnyDeposit() const
{
	for(auto && pair_l : _deposit)
	{
		if(pair_l.second)
		{
			return true;
		}
	}
	return false;
}

bool BuildingModel::isProduction() const
{
	return !_unitModels.empty();
}

bool BuildingModel::canProduce(UnitModel const * model_p) const
{
	return std::find(_unitModels.begin(), _unitModels.end(), model_p) != _unitModels.end();
}

bool BuildingModel::checkLegality(Building const &, State const &) const
{
	return true;
}

bool BuildingModel::checkGrid(Building const &building_p, State const &state_p) const
{
	return octopus::checkGrid(state_p, &building_p, false);
}

} // namespace octopus
