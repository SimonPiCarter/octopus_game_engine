#include "StepAdditionVisitor.hh"

#include "Step.hh"

#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerResetOptionDivinityStep.hh"

namespace octopus
{

StepAdditionVisitor::StepAdditionVisitor(Step &step_p) : _step(step_p)
{}

void StepAdditionVisitor::visit(PlayerSpendResourceStep const *step_p)
{
    for(auto &&pair_l : step_p->_delta)
    {
        _step.getResourceSpent(step_p->_player, pair_l.first) += pair_l.second;
    }
}

void StepAdditionVisitor::visit(PlayerResetOptionDivinityStep const *step_p)
{
    ++_step.getDivOptionSpent(step_p->_player);
}

}