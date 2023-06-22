#ifndef __RecycleDivinityStepGenerator__
#define __RecycleDivinityStepGenerator__

#include <vector>

namespace octopus
{
    class Steppable;
} // namespace octopus

std::vector<octopus::Steppable *> recycleTierOneGenertor(unsigned long player_p);
std::vector<octopus::Steppable *> recycleTierTwoGenertor(unsigned long player_p);
std::vector<octopus::Steppable *> recycleTierThreeGenertor(unsigned long player_p);

#endif