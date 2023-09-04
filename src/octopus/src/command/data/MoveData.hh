#ifndef __MoveData__
#define __MoveData__

#include "utils/Vector.hh"
#include "command/CommandData.hh"
#include <array>
#include <list>

namespace octopus
{

/// @brief Basic struct to contain flow information for massive movement
/// this struct should live as long as any command containing it lives
struct FlockInformation
{
	/// @brief number of member of the flock who reached destination
	long qtyReached = 0;
	/// @brief unique idx used to create flock unity
	unsigned long idx = 0;
	/// @brief square root of qtyReached
	Fixed sqrtQtyReached;
};

/// @brief This struct is aimed at supporting
/// move commands
class MoveData : public CommandData
{
public:
	MoveData() {}
	MoveData(Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p) :
		_finalPoint(finalPoint_p), _gridStatus(gridStatus_p) ,_waypoints(waypoints_p) {}

	///
	/// Move
	///
	/// @brief target of the move
	Vector _finalPoint;
	/// @brief number of step since update (to avoid updating too much)
	unsigned long _stepSinceUpdate {0};
	/// @brief grid status when path was computed
	unsigned long _gridStatus {0};
	// used in move
	std::list<Vector> _waypoints;
	// last position used to check if we made progress
	Vector _lastPos;
	// count steps since last progress to waypoint
	unsigned long _countSinceProgress {0};
	/// @brief boolean to know if there is los on the final point
	bool _los {false};

	// Flock
	bool _flockStarted = false;
	bool _flockReached = false;
	std::array<FlockInformation, 3> * _flockInfo {nullptr};
};

} // namespace octopus


#endif
