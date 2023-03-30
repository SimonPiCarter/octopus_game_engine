#include "StateDrawStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void StateDrawStep::apply(State &state_p) const
{
	Logger::getDebug() << "StateDrawStep :: apply " <<std::endl;
	state_p.setIsOver(true);
	state_p.setHasWinner(false);
}
void StateDrawStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "StateDrawStep :: revert " <<std::endl;
	state_p.setIsOver(_oldIsOver);
	state_p.setHasWinner(_oldhasWinningTeam);
}

}
