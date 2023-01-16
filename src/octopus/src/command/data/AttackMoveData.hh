#ifndef __AttackMoveData__
#define __AttackMoveData__

#include "utils/Vector.hh"
#include "state/Handle.hh"
#include <list>

namespace octopus
{

class EntityAttackCommand;

/// @brief This struct is aimed at supporting
/// attack, move, attack move and patrol commands
struct AttackMoveData
{
	///
	/// Attack
	///
	// target of attack
	Handle _target;
	// used in attack
	long _windup {0};


	///
	/// Move
	///
	// used in move
	std::list<Vector> _waypoints;
	// last position used to check if we made progress
	Vector _lastPos;
	// count steps since last progress to waypoint
	unsigned long _countSinceProgress {0};

	///
	/// AttackMove
	///
	// used in attackmove
	EntityAttackCommand * _subAttackCommand {nullptr};
	Vector _positionFromAttack;
};

} // namespace octopus


#endif
