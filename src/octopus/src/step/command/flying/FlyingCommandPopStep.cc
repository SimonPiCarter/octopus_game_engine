#include "FlyingCommandPopStep.hh"

#include "logger/Logger.hh"

#include "command/Command.hh"
#include "state/State.hh"

namespace octopus
{

FlyingCommandPopStep::~FlyingCommandPopStep()
{
	for(auto &&pair_l : _mapData)
	{
		delete pair_l.second;
	}
}

void FlyingCommandPopStep::apply(State &state_p) const
{
	Logger::getDebug() << "FlyingCommandPopStep :: apply " <<std::endl;

	auto &&it_l = state_p.getFlyingCommands().find(_cmd->getHandle());

	if(it_l == state_p.getFlyingCommands().end())
	{
		throw std::logic_error("Incorrect revert for flying command spawn");
	}

	_mapData[state_p._id] = it_l->second._data;
	state_p.getFlyingCommands().erase(it_l);
}

void FlyingCommandPopStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "FlyingCommandPopStep :: revert " <<std::endl;

	size_t size_l = state_p.getFlyingCommands().size();

	// reuse stored data
	state_p.getFlyingCommands().emplace_hint(state_p.getFlyingCommands().end(), _cmd->getHandle(), FlyingCommandBundle{_mapData[state_p._id], _cmd});

	// forget data stored to avoid double deletion
	_mapData[state_p._id] = nullptr;

	if(size_l == state_p.getFlyingCommands().size())
	{
		throw std::logic_error("Incorrect handle for flying command");
	}
}

void FlyingCommandPopStep::visit(SteppableVisitor * visitor_p) const
{
	visitor_p->visit(this);
}

} // namespace octopus

