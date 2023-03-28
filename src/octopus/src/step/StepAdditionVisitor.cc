#include "StepAdditionVisitor.hh"

#include "Step.hh"

#include "command/data/ProductionData.hh"
#include "step/building/BuildingCancelStep.hh"
#include "step/command/data/CancelUnitProductionStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
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

void StepAdditionVisitor::visit(EntityHitPointChangeStep const *step_p)
{
    _step.addHpChange(step_p->_handle, step_p->_delta);
}

void StepAdditionVisitor::visit(BuildingSpawnStep const *)
{
    _step.addEntitySpawned();
}

void StepAdditionVisitor::visit(BuildingCancelStep const *step_p)
{
    if(step_p->_new)
    {
        _step.addCanceledBuilding(step_p->_handle);
    }
}

void StepAdditionVisitor::visit(CancelUnitProductionStep const *step_p)
{
    _step.addCmdCanceled(step_p->_data);
}

void StepAdditionVisitor::visit(EntitySpawnStep const *)
{
    _step.addEntitySpawned();
}

void StepAdditionVisitor::visit(FlyingCommandSpawnStep const *)
{
    _step.addFlyingCommnandSpawned();
}

void StepAdditionVisitor::visit(ResourceSpawnStep const *step_p)
{
    _step.addEntitySpawned();
}

void StepAdditionVisitor::visit(UnitSpawnStep const *step_p)
{
    _step.addEntitySpawned();
}



}
