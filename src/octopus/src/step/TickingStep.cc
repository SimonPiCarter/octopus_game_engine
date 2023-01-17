#include "TickingStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void TickingStep::apply(State &state_p) const
{
	Logger::getDebug() << "TickingStep :: apply "<<std::endl;
	for(Entity *ent_l : state_p.getEntities())
	{
		++ent_l->_reload;
	}
}

void TickingStep::revert(State &state_p) const
{
	Logger::getDebug() << "TickingStep :: revert "<<std::endl;
	for(Entity *ent_l : state_p.getEntities())
	{
		if(ent_l->_reload > 0)
		{
			--ent_l->_reload;
		}
	}
}

bool TickingStep::isNoOp() const
{
	return false;
}

} // namespace octopus

