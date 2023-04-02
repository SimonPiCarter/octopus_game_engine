#ifndef __RunnerHelpers__
#define __RunnerHelpers__

#include <list>
#include <vector>
#include <fstream>

namespace octopus
{
class Command;
class Steppable;
class Library;
} // namespace octopus

namespace cuttlefish
{
class Window;
} // namespace cuttlefish



namespace fas
{
void runGame(cuttlefish::Window &window_p, std::list<octopus::Steppable *> &spawners_p,
    std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, octopus::Library &lib_p);

void runAndSaveGame(cuttlefish::Window &window_p, std::list<octopus::Steppable *> &spawners_p,
    std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, std::ofstream &file_p, octopus::Library &lib_p);

void replayGame(std::ifstream &file_p, cuttlefish::Window &window_p, std::list<octopus::Steppable *> &spawners_p,
    std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, octopus::Library const &lib_p);

void loadGame(std::ifstream &file_p, cuttlefish::Window &window_p, std::list<octopus::Steppable *> &spawners_p,
    std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, octopus::Library const &lib_p);

std::vector<octopus::Steppable*> genDialogStep(cuttlefish::Window &window_p);

} // namespace fas

#endif
