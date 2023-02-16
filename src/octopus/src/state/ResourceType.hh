#ifndef __ResourceType__
#define __ResourceType__

#include <string>

namespace octopus
{
	enum class ResourceType
	{
		// buildings/secondary ResourceType
		Steel,
		// economic/units
		Food,
		// technologies
		Gas,
		// required to anchor divinities
		Anchor,
		// required to produce fighting entities
		Ether
	};

	std::string to_string(ResourceType type_p);
} // namespace octopus


#endif
