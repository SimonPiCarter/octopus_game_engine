#ifndef __RESOURCE__
#define __RESOURCE__

#include "Entity.hh"
#include "state/model/entity/ResourceModel.hh"

namespace octopus
{

struct HarvestPoint
{
	/// @brief the point to harvest the resource
	Vector point;
	/// @brief true if there is no harvester on this point
	bool free {true};
	/// @brief the handle of the last harvest (this is used to ensure that the current harvester is still alive)
	Handle harvester;
};

class Resource : public Entity
{
	public:
		Resource(Vector const &pos_p, bool frozen_p, ResourceModel const &model_p);

		Fixed _resource {0.};

		ResourceModel const &_resourceModel;

		std::string const &getType() const { return _resourceModel._type; }

		std::vector<HarvestPoint> _harvestPoints;
};

class State;
class Step;

/// @brief check if the resource has harvest point
bool hasHarvestPoint(Resource const &resource_p);

/// @brief get the closest harvest point (using manhatan distance) for the given harvester
/// @param state_p state to check if harvesters registered are still alive
/// @param step_p step to check if a free slot have been taken already
/// @param harvester_p the harvester to use
/// @param resource_p the resoruce to use
/// @return -1 if no free harvest point or no harvest point at all
int getBestHarvestPoint(State const &state_p, Step const &step_p, Entity const &harvester_p, Resource const &resource_p);

}

#endif
