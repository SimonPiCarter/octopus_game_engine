
#include "BuildingUnitCancelCommand.hh"

#include "command/CommandVar.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/data/CancelUnitProductionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

namespace octopus
{

void BuildingUnitCancelCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingUnitCancelCommand:: register Command "<<_handleCommand <<std::endl;
	Entity const * ent_l = state_p.getEntity(_handleCommand);

	CommandData const * const data_l = octopus::getData(ent_l->getQueue().getBundle(_idx)._var);
	ProductionData const * const prodData_l = dynamic_cast<ProductionData const *>(data_l);

	if(prodData_l && !prodData_l->_canceled && !step_p.isCmdCanceled(CommandIdx(_handleCommand, _idx)))
	{
		step_p.addSteppable(new PlayerSpendResourceStep(ent_l->_player, getReverseCostMap(prodData_l->getCost())));
		step_p.addSteppable(new CancelUnitProductionStep(_handleCommand, _idx));
	}
	step_p.addSteppable(new CommandStorageStep(this));
}

} // namespace octopus

