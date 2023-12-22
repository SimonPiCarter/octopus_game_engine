#include "LoseTrigger.hh"

#include "state/State.hh"
#include "step/Step.hh"
#include "step/state/StateWinStep.hh"

namespace fas
{

LoseTrigger::LoseTrigger(octopus::Listener * listener_p) : octopus::OneShotTrigger({listener_p}) {}

void LoseTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	step_p.addSteppable(new octopus::StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 1));
}

} // godot
