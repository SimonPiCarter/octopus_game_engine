#ifndef __LifestealShowcase__
#define __LifestealShowcase__

#include <list>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> lifestealShowcaseSteps(octopus::Library &lib_p, size_t number_p);
std::list<octopus::Command *> lifestealShowcaseCommands(octopus::Library &lib_p);

#endif
