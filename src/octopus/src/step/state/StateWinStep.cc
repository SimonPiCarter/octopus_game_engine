#include "StateWinStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void StateWinStep::apply(State &state_p) const
{
	Logger::getDebug() << "StateWinStep :: apply " <<std::endl;
	state_p.setIsOver(true);
	state_p.setHasWinner(true);
	state_p.setWinningTeam(_winningTeam);
}
void StateWinStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "StateWinStep :: revert " <<std::endl;
	state_p.setIsOver(_oldIsOver);
	state_p.setHasWinner(_oldhasWinningTeam);
	state_p.setWinningTeam(_oldWinningTeam);
}

}
