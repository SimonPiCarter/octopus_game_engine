
#include "EntityBuffCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/buff/EntityBuffStep.hh"

namespace octopus
{

EntityBuffCommand::EntityBuffCommand(Handle const &commandHandle_p, Handle const &target_p, TimedBuff const &buff_p)
	: Command(commandHandle_p)
	, _target(target_p)
	, _buff(buff_p)
{}

bool EntityBuffCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Logger::getDebug() << "EntityBuffCommand:: apply Command "<<_target <<std::endl;
	// Use
	step_p.addSteppable(new EntityBuffStep(_target, _buff));

	return true;
}

} // namespace octopus

