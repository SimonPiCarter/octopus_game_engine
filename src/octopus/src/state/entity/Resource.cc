#include "Resource.hh"

namespace octopus
{

Resource::Resource(Vector const &pos_p, bool frozen_p, ResourceModel const &model_p)
	: Entity(pos_p, true, model_p)
	, _resource(model_p._qty)
	, _resourceModel(model_p)
{}

} // namespace octopus
