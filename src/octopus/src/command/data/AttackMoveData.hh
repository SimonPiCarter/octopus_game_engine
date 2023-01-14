#ifndef __AttackMoveData__
#define __AttackMoveData__

#include "utils/Vector.hh"
#include <list>


namespace octopus
{

/// @brief This struct is aimed at supporting
/// attack, move, attack move and patrol commands
struct AttackMoveData
{
	long _windup {0};
	std::list<Vector> _waypoints;
};

} // namespace octopus


#endif
