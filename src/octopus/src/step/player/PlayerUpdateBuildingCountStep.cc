#include "PlayerUpdateBuildingCountStep.hh"
#include "state/entity/Resource.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerUpdateBuildingCountStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerUpdateBuildingCountStep :: apply " << _player <<std::endl;
	for(auto &&pair_l : _delta)
	{
		state_p.getPlayer(_player)->_buildingCount[pair_l.first] += pair_l.second;
	}
}

void PlayerUpdateBuildingCountStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerUpdateBuildingCountStep :: revert " << _player <<std::endl;
	for(auto &&pair_l : _delta)
	{
		state_p.getPlayer(_player)->_buildingCount[pair_l.first] -= pair_l.second;
	}
}

bool PlayerUpdateBuildingCountStep::isNoOp() const
{
	return _delta.empty();
}

} // namespace octopus
