#ifndef __CommandHelpers__
#define __CommandHelpers__

#include "Command.hh"
#include "utils/Vector.hh"

namespace octopus
{
class Controller;
class State;

Command * newTargetCommand(State const &state_p, Handle const &source_l, Handle const &target_p, Vector const &pos_p, bool targetNotSet_p);

} // namespace octopus


#endif
