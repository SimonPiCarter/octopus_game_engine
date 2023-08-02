
#include "DebugCommand.hh"

#include "logger/Logger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"

namespace octopus
{

DebugCommand::DebugCommand(Library &lib_p)
	: Command(Handle(0,0))
	, _lib(&lib_p)
{}

void DebugCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "DebugCommand:: register Command "<<std::endl;
	// step_p.addSteppable(steppable_l);

	Unit unit_l({42,10}, false, _lib->getUnitModel("triangle"));
	unit_l._player = 1;
	Handle handle_l = getNextHandle(step_p, state_p);
	step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
}

bool DebugCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	// NA
	return true;
}

} // namespace octopus

