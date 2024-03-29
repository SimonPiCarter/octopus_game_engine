#ifndef __EntityAttackMoveCommand__
#define __EntityAttackMoveCommand__

#include "command/Command.hh"
#include "EntityMoveCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"
#include <list>

namespace octopus
{

class Entity;

/// @brief This command will move along waypoints and will attack targets
/// that come into range while returning to its path once target are missing
/// or too far
class EntityAttackMoveCommand : public Command
{
public:
	EntityAttackMoveCommand() {}
	EntityAttackMoveCommand(Handle const &commandHandle_p, Handle const &source_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false, bool neverStop_p=false);

	void setFlockInformation(std::array<FlockInformation, 3> * flockInfo_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * getData() override { return &_data; }
	virtual CommandData const * getData() const override { return &_data; }

	virtual void cleanUp(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	EntityMoveCommand const &getSubMoveCommand() const { return _subMoveCommand; }
	EntityMoveCommand &getSubMoveCommand() { return _subMoveCommand; }

private:
	Handle _source {0};

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;

	AttackMoveData _data;
};

} // namespace octopus


#endif
