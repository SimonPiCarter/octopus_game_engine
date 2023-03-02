#ifndef __EntityAttackCommand__
#define __EntityAttackCommand__

#include "command/Command.hh"

#include "command/data/AttackMoveData.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"
#include <list>


namespace octopus
{

/// @brief This command will move in range of the target then attack it
/// it targets die will search for new targets in a radius of aggression
/// Will move to the closest point in range of the target when moving
class EntityAttackCommand : public Command
{
public:
	EntityAttackCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * newData() const override { return new AttackMoveData(_target, Vector{}, 0, {}); }

private:
	Handle const _source;
	/// @brief initial target
	Handle _target;

	bool checkTarget(State const &state_p, Handle const & target_p) const;
	bool inRange(State const &state_p, Handle const & target_p) const;
};

} // namespace octopus


#endif
