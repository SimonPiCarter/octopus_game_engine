#ifndef __AttackMoveData__
#define __AttackMoveData__

#include "utils/Vector.hh"
#include <list>


namespace octopus
{

class EntityAttackCommand;

/// @brief This struct is aimed at supporting
/// attack, move, attack move and patrol commands
struct AttackMoveData
{
	// used in attack
	long _windup {0};
	// used in move
	std::list<Vector> _waypoints;

	// used in attackmove
	EntityAttackCommand * _subAttackCommand {nullptr};
	Vector _positionFromAttack;
};

} // namespace octopus


#endif
