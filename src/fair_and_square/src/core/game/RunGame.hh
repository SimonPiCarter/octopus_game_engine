#ifndef __RunGame__
#define __RunGame__

namespace cuttlefish
{
    class Window;

} // namespace cuttlefish



namespace fas
{
    void runWave(cuttlefish::Window &window_p, unsigned long stepCount_p, unsigned long player_p);

    void runArena(cuttlefish::Window &window_p);
} // namespace fas



#endif