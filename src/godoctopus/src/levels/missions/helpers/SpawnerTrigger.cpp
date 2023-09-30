#include "SpawnerTrigger.h"

#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"

template<>
octopus::Steppable * makeSpawnStep(octopus::Handle const &handle_p, octopus::Unit const &ent_p)
{
	return new octopus::UnitSpawnStep(handle_p, ent_p);
}

template<>
octopus::Steppable * makeSpawnStep(octopus::Handle const &handle_p, octopus::Building const &ent_p)
{
	return new octopus::BuildingSpawnStep(handle_p, ent_p, true);
}

template<>
octopus::Steppable * makeSpawnStep(octopus::Handle const &handle_p, octopus::Resource const &ent_p)
{
	return new octopus::ResourceSpawnStep(handle_p, ent_p);
}

template<>
octopus::Steppable * makeSpawnStep(octopus::Handle const &handle_p, octopus::Entity const &ent_p)
{
	return new octopus::EntitySpawnStep(handle_p, ent_p);
}
