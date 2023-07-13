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
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false, bool neverStop_p=false);

	void setFlockInformation(FlockInformation * flockInfo_p);

	/// @brief check if _ent is close to the waypoint, if so
	/// remove reached waypoint and create a step to the next waypoint
	/// multiple waypoint may be removed at once last waypoint
	/// is removed terminate
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, CommandContext &commandContext_p) const override;

	Vector const &getFinalPoint() const { return _finalPoint; }
	unsigned long const &getGridStatus() const { return _gridStatus; }
	std::list<Vector> const &getWaypoints() const { return _waypoints; }
	bool isInit() const { return _init; }
	bool isNeverStop() const { return _neverStop; }

	virtual CommandData * getData() override { return &_data; }
	virtual CommandData const * getData() const override { return &_data; }
private:
	Handle _source {0};
	Vector _finalPoint {1,1};
	unsigned long _gridStatus {0};
	std::list<Vector> _waypoints;
	bool _init {false};
	/// @brief if set to true the command will never stop when not making progress
	bool _neverStop {false};

	// data to be updated
	MoveData _data;
};

} // namespace octopus


#endif
