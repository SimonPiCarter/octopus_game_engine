#ifndef __EntityRallyPointCommand__
#define __EntityRallyPointCommand__

#include "command/Command.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"

namespace octopus
{

/// @brief Command that make a entity execute the rally point
/// need a specific command because entity was not created when this command was commited
class EntityRallyPointCommand : public Command
{
public:
	EntityRallyPointCommand() {}
	EntityRallyPointCommand(Handle const &source_p, Handle const &target_p, Vector const &pos_p, bool targetNotSet_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *, CommandContext &) const override;

private:
    Handle _target {0};
    Vector _pos {0,0};
    bool _targetNotSet {false};
};

} // namespace octopus


#endif
