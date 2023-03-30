#include "StateTemplePositionAddStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void StateTemplePositionAddStep::apply(State &state_p) const
{
	Logger::getDebug() << "StateTemplePositionAddStep :: apply " << _pos <<std::endl;
	state_p.getTemplePosition().push_back(_pos);
}
void StateTemplePositionAddStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "StateTemplePositionAddStep :: revert " << _pos <<std::endl;
	state_p.getTemplePosition().remove(_pos);
}

}
