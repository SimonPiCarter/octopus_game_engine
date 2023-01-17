#ifndef __AttackMoveData__
#define __AttackMoveData__

#include "MoveData.hh"
#include "state/Handle.hh"
#include <list>

namespace octopus
{

class EntityAttackCommand;

/// @brief This struct is aimed at supporting
/// attack, move, attack move and patrol commands
struct AttackMoveData : public MoveData
{
	AttackMoveData(Handle const &target_p, std::list<Vector> const &waypoints_p) : MoveData(waypoints_p), _target(target_p) {}
	///
	/// Attack
	///
	// target of attack
	Handle _target;
	// used in attack
	long _windup {0};

	///
	/// AttackMove
	///
	// used in attackmove
	EntityAttackCommand * _subAttackCommand {nullptr};
	Vector _positionFromAttack;
};

} // namespace octopus


#endif
