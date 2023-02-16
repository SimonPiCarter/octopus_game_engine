#include "ResourceType.hh"

namespace octopus
{
std::string to_string(ResourceType type_p)
{
	switch(type_p)
	{
		case ResourceType::Steel:
			return "Steel";
		case ResourceType::Food:
			return "Food";
		case ResourceType::Gas:
			return "Gas";
		case ResourceType::Anchor:
			return "Anchor";
		case ResourceType::Ether:
			return "Ether";
	}
	return "";
}
} // namespace octopus

