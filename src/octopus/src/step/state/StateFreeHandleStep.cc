#include "StateFreeHandleStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void StateFreeHandleStep::apply(State &state_p) const
{
	Logger::getDebug() << "StateFreeHandleStep :: apply " <<std::endl;
	for(Handle const &handle_l : _freeHandles)
	{
		state_p.addFreeHandle(handle_l);
	}
}
void StateFreeHandleStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "StateFreeHandleStep :: revert " <<std::endl;
	for(Handle const &handle_l : _freeHandles)
	{
		state_p.popBackFreeHandle();
	}
}

}

