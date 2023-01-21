#ifndef __Cases__
#define __Cases__

#include <list>
namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus


std::list<octopus::Steppable *> Case1(octopus::Library &lib_p);
std::list<octopus::Steppable *> Case2(octopus::Library &lib_p);

#endif
