#include "CommandDataLongStep.hh"

#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandDataLongDiffStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataLongDiffStep :: apply " << this->_handle <<std::endl;
	CommandDataWithData<long> *data_l = dynamic_cast<CommandDataWithData<long> *>(ent_l->getFrontQueue()._data);
	data_l->_data += _diff;
}

void CommandDataLongDiffStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataLongDiffStep :: revert " << this->_handle <<std::endl;
	CommandDataWithData<long> *data_l = dynamic_cast<CommandDataWithData<long> *>(ent_l->getFrontQueue()._data);
	data_l->_data -= _diff;
}

bool CommandDataLongDiffStep::isNoOp() const
{
	return _diff == 0;
}

} // namespace octopus
