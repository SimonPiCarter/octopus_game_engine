#ifndef __RunLevel1__
#define __RunLevel1__

#include <cstddef>

namespace cuttlefish
{
    class Window;

} // namespace cuttlefish



namespace fas
{
    void runLevel1(cuttlefish::Window &window_p, unsigned long stepCount_p, unsigned long worldSize_p, unsigned long seed_p);

    void replayLevel1(cuttlefish::Window &window_p);

    void loadLevel1(cuttlefish::Window &window_p);
} // namespace fas



#endif
