#ifndef __UnitModel__
#define __UnitModel__

#include <map>

#include "EntityModel.hh"

namespace octopus
{
	struct UnitModel : public EntityModel
	{
		std::map<ResourceType, double> _maxQuantity;
	};
} // namespace octopus


#endif
