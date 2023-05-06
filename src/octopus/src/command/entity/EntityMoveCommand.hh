#ifndef __EntityMoveCommand__
#define __EntityMoveCommand__

#include "command/Command.hh"

#include "command/data/MoveData.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"
#include <list>


namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityMoveCommand : public Command
{
public:
	EntityMoveCommand() {}
	EntityMoveCommand(Handle const &commandHandle_p, Handle const &source_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false);

	/// @brief check if _ent is close to the waypoint, if so
	/// remove reached waypoint and create a step to the next waypoint
	/// multiple waypoint may be removed at once last waypoint
	/// is removed terminate
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	Vector const &getFinalPoint() const { return _finalPoint; }
	unsigned long const &getGridStatus() const { return _gridStatus; }
	std::list<Vector> const &getWaypoints() const { return _waypoints; }
	bool isInit() const { return _init; }

	virtual CommandData * newData() const override { return new MoveData(_finalPoint, _gridStatus, _waypoints); }
private:
	Handle _source {0};
	Vector _finalPoint {1,1};
	unsigned long _gridStatus {0};
	std::list<Vector> _waypoints;
	bool _init {false};
};

} // namespace octopus


#endif
