#include "TempleModel.hh"

#include "state/State.hh"
#include "state/entity/Building.hh"

namespace octopus
{

bool TempleModel::checkLegality(Building const &building_p, State const &state_p) const
{
	std::list<Vector> const &list_l = state_p.getTemplePosition();

	for(Vector const &v : list_l)
	{
		if(square_length(v - building_p._pos) < 1e-5)
		{
			return true;
		}
	}
	return false;
}

bool TempleModel::checkGrid(Building const &building_p, State const &state_p) const
{
	return octopus::checkGrid(state_p, &building_p, true);
}

} // namespace octopus
