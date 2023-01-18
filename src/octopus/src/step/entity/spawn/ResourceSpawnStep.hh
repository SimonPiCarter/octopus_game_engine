#ifndef __RESOURCE_SPAWN_STEP__
#define __RESOURCE_SPAWN_STEP__

#include "SpawnModelStep.hh"
#include "state/entity/Resource.hh"

namespace octopus
{
class ResourceSpawnStep : public SpawnModelStep<Resource>
{
public:
	explicit ResourceSpawnStep(Resource const &model_p);
};
}

#endif
