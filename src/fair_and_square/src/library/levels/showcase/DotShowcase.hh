#ifndef __DotShowcase__
#define __DotShowcase__

#include <cstddef>
#include <cstdint>
#include <list>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> dotShowcaseSteps(octopus::Library &lib_p, uint32_t number_p);
std::list<octopus::Command *> dotShowcaseCommands(octopus::Library &lib_p);

#endif
