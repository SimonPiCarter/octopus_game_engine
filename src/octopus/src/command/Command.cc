#include "Command.hh"
#include "Commandable.hh"
#include "state/State.hh"

namespace octopus
{
void Command::registerCommand(State &state_p)
{
	Commandable * cmdable_l = state_p.getCommandable(_handle);
	cmdable_l->enqueue(this, _queued);
}
}
