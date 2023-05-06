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
	AttackMoveData() {}
	AttackMoveData(Handle const &target_p, Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p)
		: MoveData(finalPoint_p, gridStatus_p, waypoints_p), _target(target_p) {}
	~AttackMoveData();
	/// Attack
	///
	// target of attack
	Handle _target {0};
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
