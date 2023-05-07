#ifndef __AttackData__
#define __AttackData__

#include "MoveData.hh"
#include "state/Handle.hh"
#include <list>

namespace octopus
{

/// @brief This struct is aimed at supporting
/// attack, move, attack move and patrol commands
struct AttackData : public MoveData
{
	AttackData() {}
	AttackData(Handle const &target_p, Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p)
		: MoveData(finalPoint_p, gridStatus_p, waypoints_p), _target(target_p) {}
	// target of attack
	Handle _target {0};
	// used in attack
	long _windup {0};
};

} // namespace octopus


#endif
