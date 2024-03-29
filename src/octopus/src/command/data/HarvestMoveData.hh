#ifndef __HarvestMoveData__
#define __HarvestMoveData__

#include "MoveData.hh"
#include "state/Handle.hh"
#include <list>

namespace octopus
{

class EntityAttackCommand;

/// @brief This struct is aimed at supporting
/// harvest and move
struct HarvestMoveData : public MoveData
{
	HarvestMoveData() {}
	HarvestMoveData(Handle const &resource_p, Handle const &deposit_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p)
		: MoveData(finalPoint_p, gridStatus_p, waypoints_p), _resource(resource_p), _deposit(deposit_p)
	{
		_ignoreCollision = true;
	}

	Handle _resource {0};
	/// @brief slot to be used on the resource
	int _idxSlot {-1};
	Handle _deposit {0};
	/// @brief boolean to tell if on the way to deposit or not
	bool _harvesting { true };

	/// @brief time since harvest
	Fixed _timeSinceHarvest {0.};
};

} // namespace octopus


#endif
