#ifndef __Cases__
#define __Cases__

#include <list>
namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus

/// @brief mass unit movement test
std::list<octopus::Steppable *> Case1(octopus::Library &lib_p);
/// @brief small harvesting test
std::list<octopus::Steppable *> Case2(octopus::Library &lib_p);
/// @brief production test
std::list<octopus::Steppable *> Case3(octopus::Library &lib_p);

#endif
