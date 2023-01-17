#ifndef __MoveData__
#define __MoveData__

#include "utils/Vector.hh"
#include "command/CommandData.hh"
#include <list>

namespace octopus
{

/// @brief This struct is aimed at supporting
/// move commands
class MoveData : public CommandData
{
public:
	MoveData(std::list<Vector> const &waypoints_p) : _waypoints(waypoints_p) {}
	///
	/// Move
	///
	// used in move
	std::list<Vector> _waypoints;
	// last position used to check if we made progress
	Vector _lastPos;
	// count steps since last progress to waypoint
	unsigned long _countSinceProgress {0};
};

} // namespace octopus


#endif
