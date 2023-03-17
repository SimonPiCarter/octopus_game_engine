#include "FlyingCommandSpawnStep.hh"

#include "logger/Logger.hh"

#include "command/Command.hh"
#include "state/State.hh"

namespace octopus
{

void FlyingCommandSpawnStep::apply(State &state_p) const
{
	Logger::getDebug() << "FlyingCommandSpawnStep :: apply " <<std::endl;

	size_t size_l = state_p.getFlyingCommands().size();

	state_p.getFlyingCommands().emplace_hint(state_p.getFlyingCommands().end(), _cmd->getHandle(), FlyingCommandBundle{_cmd->newData(), _cmd});

	if(size_l == state_p.getFlyingCommands().size())
	{
		throw std::logic_error("Incorrect handle for flying command");
	}
}
void FlyingCommandSpawnStep::revert(State &state_p) const
{
	Logger::getDebug() << "FlyingCommandSpawnStep :: revert " <<std::endl;

	auto &&it_l = state_p.getFlyingCommands().find(_cmd->getHandle());

	if(it_l == state_p.getFlyingCommands().end())
	{
		throw std::logic_error("Incorrect revert for flying command spawn");
	}

	delete it_l->second._data;
	state_p.getFlyingCommands().erase(it_l);
}

void FlyingCommandSpawnStep::visit(SteppableVisitor * visitor_p) const
{
	visitor_p->visit(this);
}

} // namespace octopus

