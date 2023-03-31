#ifndef __AoeShowcase__
#define __AoeShowcase__

#include <list>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> aoeShowcaseSteps(octopus::Library &lib_p, size_t number_p);
std::list<octopus::Command *> aoeShowcaseCommands(octopus::Library &lib_p);

#endif
