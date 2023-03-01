
#include "EntityBuffCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/buff/EntityBuffStep.hh"

namespace octopus
{

EntityBuffCommand::EntityBuffCommand(Handle const &commandHandle_p, Handle const &target_p, TyppedBuff const &buff_p)
	: Command(commandHandle_p)
	, _target(target_p)
	, _buff(buff_p)
{}

bool EntityBuffCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Logger::getDebug() << "EntityBuffCommand:: apply Command "<<_target <<std::endl;
	Entity const * ent_l = state_p.getEntity(_target);
	auto &&it_l = ent_l->_timeSinceBuff.find(_buff._id);
	bool new_l = it_l == ent_l->_timeSinceBuff.cend();
	unsigned long time_l = 0;
	if(!new_l)
	{
		time_l = it_l->second;
	}
	// Use
	step_p.addSteppable(new EntityBuffStep(_target, _buff, time_l, new_l ));

	return true;
}

} // namespace octopus

