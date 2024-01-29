#ifndef __StateDump__
#define __StateDump__

#include <iostream>

namespace octopus
{

class State;

/// @brief write state information as simple text to the given output stream
/// will stream information
/// - Players
///   - Amount of resources
///   - upgrade level
/// - Entities
///   - Hit point
///   - Position
///   - Alive (1/0)
void streamStateAsSimpleText(std::ostream &os_p, State const &state_p);

std::string hashState(State const &state_p);

} // namespace octopus


#endif