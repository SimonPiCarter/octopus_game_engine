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

/// @brief represent the unlock routine state (1 and 2 tells if we are going "left" or "right", SMALL,MIDDLE,LONG tells by how much we move on the side)
enum class UnlockRoutineState { SMALL_1, SMALL_2, MIDDLE_1, MIDDLE_2, LONG_1, LONG_2, NONE };
/// @brief Basic struct to store the unlock routine state in move data
struct UnlockRoutine
{
	/// @brief current state
	UnlockRoutineState _unlockState = UnlockRoutineState::NONE;
	/// @brief the target point of the unlock state
	Vector _targetPoint;
	/// @brief the step id when the current state was started to know when to stop it
	unsigned long long _stepStartId = 0;
	/// @brief the step if when to stop the current unlock routine (to avoid getting stuck)
	unsigned long long _stepEndId = 0;
	/// @brief true if the unlock routine is enabled right now (meaning we want to move to its target point)
	bool _enabled = false;
	/// @brief true when we are trying a direct move to the original destination to check if we succesfully unlocked
	bool _tryout = false;
	/// @brief
	Vector _tryoutPoint = Vector(0,0);

	static unsigned long MaxStepState;
	static unsigned long SmallRange;
	static unsigned long MiddleRange;
	static unsigned long LongRange;
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

	/// @brief data required for unblock routine
	UnlockRoutine _unlockRoutine;

	// Flock
	bool _flockStarted = false;
	bool _flockReached = false;
	std::array<FlockInformation, 3> * _flockInfo {nullptr};
};

} // namespace octopus


#endif
