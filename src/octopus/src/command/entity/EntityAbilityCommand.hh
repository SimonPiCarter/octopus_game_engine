#ifndef __EntityAbilityCommand__
#define __EntityAbilityCommand__

#include "command/Command.hh"
#include "state/Handle.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "utils/Vector.hh"

#include <functional>

namespace octopus
{

/// @brief Ability command
/// an ability command rely on a custom function to
/// apply steps
class EntityAbilityCommand : public Command
{
public:
	EntityAbilityCommand(Handle const &commandHandle_p, Handle const &target_p, Vector const &pointTarget_p, std::string const &id_p);

	/// @brief register the command into the step
	/// This method is responsible for
	/// handling cost of command and spawning command in step
	/// and checking legality
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief compile command or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, CommandContext &commandContext_p) const override;

	Handle _target {0};
	Vector _pointTarget;
	std::string _id;
};

} // namespace octopus


#endif
