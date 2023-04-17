#ifndef __Godoctopus_ProductionCommand__
#define __Godoctopus_ProductionCommand__

#include <cstdint>

#include "state/Handle.hh"

namespace octopus
{
    class BuildingUnitProductionCommand;
    class UnitProductionData;
} // namespace octopus


namespace godot {

struct CommandInfo
{
	octopus::BuildingUnitProductionCommand const * cmd;
	octopus::UnitProductionData const * data;
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
