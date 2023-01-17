#ifndef __ResourceType__
#define __ResourceType__

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
} // namespace octopus


#endif
