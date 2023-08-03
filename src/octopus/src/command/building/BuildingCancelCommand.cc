
#include "BuildingCancelCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/EntityModel.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/data/CancelUnitProductionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/building/BuildingCancelStep.hh"

namespace octopus
{

void BuildingCancelCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingCancelCommand:: register Command "<<_handleCommand <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_handleCommand));

	if(!building_l->isBuilt()
	&& !building_l->_canceled
	&& !step_p.isCanceled(_handleCommand))
	{
		Logger::getDebug() << "BuildingCancelCommand:: canceling "<<_handleCommand<<std::endl;
		step_p.addSteppable(new BuildingCancelStep(_handleCommand, building_l->_canceled, true));
		step_p.addSteppable(new PlayerSpendResourceStep(building_l->_player, getReverseCostMap(building_l->_model._cost)));
	}
}

} // namespace octopus

