
#include "BuildingUnitCancelCommand.hh"

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

	UnitProductionData const * const data_l = dynamic_cast<UnitProductionData const *>(ent_l->getQueue().getBundle(_idx)._data);

	if(data_l && !data_l->_canceled && !step_p.isCmdCanceled(CommandIdx(_handleCommand, _idx)))
	{
		step_p.addSteppable(new PlayerSpendResourceStep(ent_l->_player, getReverseCostMap(data_l->_model._cost)));
		step_p.addSteppable(new CancelUnitProductionStep(_handleCommand, _idx));
	}
	step_p.addSteppable(new CommandStorageStep(this));
}

} // namespace octopus

