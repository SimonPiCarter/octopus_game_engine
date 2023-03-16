#include "PlayerSpendResourceStep.hh"
#include "state/entity/Resource.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerSpendResourceStep::apply(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "PlayerSpendResourceStep :: apply " << _player <<std::endl;
	for(auto &&pair_l : _delta)
	{
		state_p.getPlayer(_player)->_resources[pair_l.first] -= pair_l.second;
	}
}

void PlayerSpendResourceStep::revert(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "PlayerSpendResourceStep :: revert " << _player <<std::endl;
	for(auto &&pair_l : _delta)
	{
		state_p.getPlayer(_player)->_resources[pair_l.first] += pair_l.second;
	}
}

bool PlayerSpendResourceStep::isNoOp() const
{
	return _delta.empty();
}

} // namespace octopus
