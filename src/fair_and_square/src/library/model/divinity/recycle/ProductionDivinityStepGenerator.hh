#ifndef __ProductionDivinityStepGenerator__
#define __ProductionDivinityStepGenerator__

#include <vector>

namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus

std::vector<octopus::Steppable *> productionTierOneGenertor(unsigned long player_p);
std::vector<octopus::Steppable *> productionTierTwoGenertor(octopus::Library const &lib_p, unsigned long player_p);
std::vector<octopus::Steppable *> productionTierThreeGenertor(octopus::Library const &lib_p, unsigned long player_p);

#endif
