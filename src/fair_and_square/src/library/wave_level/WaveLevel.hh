#ifndef __WaveLevel__
#define __WaveLevel__

#include <list>

namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus


std::list<octopus::Steppable *> WaveLevel(octopus::Library &lib_p);

#endif