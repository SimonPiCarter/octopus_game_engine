#include "StateTemplePositionRemoveStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void StateTemplePositionRemoveStep::apply(State &state_p) const
{
	Logger::getDebug() << "StateTemplePositionRemoveStep :: apply " << _pos <<std::endl;
	state_p.getTemplePosition().remove(_pos);
}
void StateTemplePositionRemoveStep::revert(State &state_p) const
{
	Logger::getDebug() << "StateTemplePositionRemoveStep :: revert " << _pos <<std::endl;
	state_p.getTemplePosition().push_back(_pos);
}

}
