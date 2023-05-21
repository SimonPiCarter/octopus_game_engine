#ifndef __Godoctopus_ProductionCommand__
#define __Godoctopus_ProductionCommand__

#include <cstddef>

#include "state/Handle.hh"

namespace octopus
{
    class ProductionData;
} // namespace octopus


namespace godot {

struct CommandInfo
{
	octopus::Handle handle;
	octopus::ProductionData const * data;
	size_t idx;
	/// @brief used for sorting purpose
	size_t posInQueue;
};


struct CommandSorter
{
	bool operator()(CommandInfo const &a, CommandInfo const &b) const;
};

}

#endif //__Godoctopus_ProductionCommand__
