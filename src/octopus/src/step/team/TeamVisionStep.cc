#include "TeamVisionStep.hh"
#include "state/entity/Resource.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "state/vision/VisionGrid.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TeamVisionStep::apply(State &state_p) const
{
	Logger::getDebug() << "TeamVisionStep :: apply " << _team <<std::endl;

	VisionGrid & grid_l = *getVisionGrid(state_p, _team);

	for(std::pair<long, long> const &pair_l : _pattern)
	{
        unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first), grid_l.getSize()-1));
        unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second), grid_l.getSize()-1));
        grid_l.incrementVisionGrid(x, y, _increase?1:-1, _exploration);
	}
}

void TeamVisionStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "TeamVisionStep :: revert " << _team <<std::endl;

	VisionGrid & grid_l = *getVisionGrid(state_p, _team);

	for(std::pair<long, long> const &pair_l : _pattern)
	{
        unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first), grid_l.getSize()-1));
        unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second), grid_l.getSize()-1));
        grid_l.incrementVisionGrid(x, y, _increase?-1:1, _exploration);
	}
}

bool TeamVisionStep::isNoOp() const
{
	return _pattern.empty();
}

} // namespace octopus
