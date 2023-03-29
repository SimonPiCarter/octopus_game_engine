#include "BuildingSpawnStep.hh"

namespace octopus
{

BuildingSpawnStep::BuildingSpawnStep(Handle const &handle_p, Building const &model_p, bool builtAlready_p)
	: SpawnModelStep(handle_p, model_p, builtAlready_p)
{}


void BuildingSpawnStep::apply(State &state_p) const
{
	SpawnModelStep::apply(state_p);

	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	// if build update building process
	if(_forceAlive)
	{
		ent_l->_buildingProgress = ent_l->_buildingModel._buildingTime;
	}
}

} // namespace octopus

