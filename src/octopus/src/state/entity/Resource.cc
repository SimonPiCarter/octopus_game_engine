#include "Resource.hh"

#include "state/State.hh"
#include "step/Step.hh"

namespace octopus
{

Resource::Resource(Vector const &pos_p, bool frozen_p, ResourceModel const &model_p)
	: Entity(pos_p, true, model_p)
	, _resource(model_p._qty)
	, _resourceModel(model_p)
{
	// initialize harvest points
	for(Vector const &point_l : _resourceModel._harvestPoints)
	{
		HarvestPoint harvestPoint_l;
		harvestPoint_l.point = point_l;
		_harvestPoints.push_back(harvestPoint_l);
	}
}

bool hasHarvestPoint(Resource const &resource_p)
{
	return !resource_p._harvestPoints.empty();
}

bool isHarvestPointFree(HarvestPoint const &point_p, State const &state_p)
{
	return point_p.free || !state_p.isEntityAlive(point_p.harvester);
}

int getBestHarvestPoint(State const &state_p, Step const &step_p, Entity const &harvester_p, Resource const &resource_p)
{
	int result_l = -1;
	Fixed best_l;

	for(int i = 0 ; i < resource_p._harvestPoints.size() ; ++i)
	{
		HarvestPoint const &point_l = resource_p._harvestPoints[i];

		// skip non free points
		if(!isHarvestPointFree(point_l, state_p) || step_p.isSlotTaken(resource_p._handle, i))
		{
			continue;
		}

		// harvest point using position of the resource
		Vector adjustedPoint_l = resource_p.getHarvestPoint(i);
		Fixed manDistance_l = numeric::abs(harvester_p._pos.x - adjustedPoint_l.x) + numeric::abs(harvester_p._pos.y - adjustedPoint_l.y);

		if(result_l < 0 || best_l > manDistance_l)
		{
			result_l = i;
			best_l = manDistance_l;
		}
	}

	return result_l;
}


} // namespace octopus
