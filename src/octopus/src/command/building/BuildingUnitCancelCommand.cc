
#include "BuildingUnitCancelCommand.hh"

#include "command/CommandVar.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/data/CancelUnitProductionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerProducedUpgradeStep.hh"

namespace octopus
{

void BuildingUnitCancelCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingUnitCancelCommand:: register Command "<<_handleCommand <<std::endl;
	Entity const * ent_l = state_p.getEntity(_handleCommand);

	step_p.addSteppable(state_p, new CommandStorageStep(this));
	// get bundle
	CommandBundle const *bundle_l = ent_l->getQueue().getBundle(_idx);
	// if bundle is null means that the command was already cancelled and cleaned up
	if(!bundle_l)
	{
		return;
	}
	CommandData const * const data_l = octopus::getData(bundle_l->_var);
	ProductionData const * const prodData_l = dynamic_cast<ProductionData const *>(data_l);

	if(prodData_l && !prodData_l->_canceled && !step_p.isCmdCanceled(CommandIdx(_handleCommand, _idx)))
	{
		Player const &player_l = *state_p.getPlayer(ent_l->_player);
		step_p.addSteppable(state_p, new PlayerSpendResourceStep(ent_l->_player, getReverseCostMap(prodData_l->getCost(player_l))));
		step_p.addSteppable(state_p, new CancelUnitProductionStep(_handleCommand, _idx));
		// if update remove update being processed
		UpgradeProductionData const * upgradeData_l = dynamic_cast<UpgradeProductionData const *>(data_l);
		if(upgradeData_l)
		{
			step_p.addSteppable(state_p, new PlayerProducedUpgradeStep(ent_l->_player, upgradeData_l->_upgrade->_id, false));
		}
	}
}

} // namespace octopus

