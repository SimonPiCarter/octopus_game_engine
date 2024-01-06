#ifndef __AoeShowcase__
#define __AoeShowcase__

#include <cstddef>
#include <list>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> aoeShowcaseSteps(octopus::Library &lib_p, uint32_t number_p);
std::list<octopus::Command *> aoeShowcaseCommands(octopus::Library &lib_p);

#endif
