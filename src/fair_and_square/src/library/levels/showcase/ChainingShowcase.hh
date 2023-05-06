#ifndef __ChainingShowcase__
#define __ChainingShowcase__

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


std::list<octopus::Steppable *> chainingShowcaseSteps(octopus::Library &lib_p);
std::list<octopus::Command *> chainingShowcaseCommands(octopus::Library &lib_p);

#endif
