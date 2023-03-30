#include "TimerDataUpdateStep.hh"

#include <cassert>

#include "command/data/TimerData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void TimerDataUpdateStep::apply(State &state_p) const
{
	auto && it_l = state_p.getFlyingCommands().find(this->_handle);
	assert(it_l != state_p.getFlyingCommands().end());

	Logger::getDebug() << "TimerDataUpdateStep :: apply " << this->_handle <<std::endl;

	TimerData *data_l = dynamic_cast<TimerData*>(it_l->second._data);
	if(data_l->_time != _old)
	{
		throw std::logic_error("Setting target had incoherent old target regarding content in state");
	}
	data_l->_time = _new;
}

void TimerDataUpdateStep::revert(State &state_p, SteppableData const *) const
{
	auto && it_l = state_p.getFlyingCommands().find(this->_handle);
	assert(it_l != state_p.getFlyingCommands().end());

	Logger::getDebug() << "TimerDataUpdateStep :: revert " << this->_handle <<std::endl;
	TimerData *data_l = dynamic_cast<TimerData*>(it_l->second._data);
	if(data_l->_time != _new)
	{
		throw std::logic_error("Setting target had incoherent old target regarding content in state");
	}
	data_l->_time = _old;
}

bool TimerDataUpdateStep::isNoOp() const
{
	return _new == _old;
}

} // namespace octopus
