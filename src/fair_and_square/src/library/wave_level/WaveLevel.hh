#ifndef __WaveLevel__
#define __WaveLevel__

#include <list>

namespace octopus
{
	class Command;
	class Steppable;
	class Library;
} // namespace octopus


std::list<octopus::Steppable *> WaveLevelSteps(octopus::Library &lib_p);
std::list<octopus::Command *> WaveLevelCommands(octopus::Library &lib_p);

#endif