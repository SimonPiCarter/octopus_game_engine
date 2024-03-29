#ifndef __EntityAttackCommand__
#define __EntityAttackCommand__

#include "command/Command.hh"

#include "command/data/AttackData.hh"
#include "command/entity/EntityMoveCommand.hh"
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
	EntityAttackCommand() {}
	EntityAttackCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p, bool frozenTarget_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * getData() override { return &_data; }
	virtual CommandData const * getData() const override { return &_data; }

	virtual void cleanUp(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	Handle const &getSource() const;
	Handle const &getTarget() const;
	bool isFrozenTarget() const;
private:
	Handle _source {0};
	/// @brief initial target
	Handle _target {0};
	/// @brief if true we wont change target except when it dies
	bool _frozenTarget {false};

	AttackData _data;

	EntityMoveCommand _moveCommand;

	/// @brief return true if the target is not valid anymore
	bool checkTarget(State const &state_p, Handle const & target_p, bool healing_p) const;
	bool inRange(State const &state_p, Handle const & target_p) const;
};

} // namespace octopus


#endif
