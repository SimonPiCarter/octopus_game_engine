#ifndef __DebugCommand__
#define __DebugCommand__

#include "command/Command.hh"
#include "state/Handle.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "utils/Vector.hh"

#include <functional>

namespace octopus
{

class Library;
class Steppable;

/// @brief Ability command
/// an ability command rely on a custom function to
/// apply steps
class DebugCommand : public Command
{
public:
	DebugCommand(Library &lib_p);

	/// @brief register the command into the step
	/// This method is responsible for
	/// handling cost of command and spawning command in step
	/// and checking legality
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief compile command or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

private:
	Library const *_lib;
};

} // namespace octopus


#endif
