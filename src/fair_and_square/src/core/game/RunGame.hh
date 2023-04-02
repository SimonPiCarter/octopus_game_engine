#ifndef __RunGame__
#define __RunGame__

#include <cstddef>

namespace cuttlefish
{
    class Window;

} // namespace cuttlefish



namespace fas
{
    void runWave(cuttlefish::Window &window_p, unsigned long stepCount_p, unsigned long player_p, unsigned long worldSize_p, unsigned long seed_p);

    void replayWave(cuttlefish::Window &window_p);

    void loadWave(cuttlefish::Window &window_p);

    void runArena(cuttlefish::Window &window_p, unsigned long number_p);

    void replayArena(cuttlefish::Window &window_p);

    void loadArena(cuttlefish::Window &window_p);

    void runMaze(cuttlefish::Window &window_p, unsigned long number_p);
} // namespace fas



#endif
