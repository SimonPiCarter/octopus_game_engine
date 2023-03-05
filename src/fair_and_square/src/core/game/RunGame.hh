#ifndef __RunGame__
#define __RunGame__

namespace cuttlefish
{
    class Window;

} // namespace cuttlefish



namespace fas
{
    void runGame(cuttlefish::Window &window_p, unsigned long stepCount_p, unsigned long player_p);
} // namespace fas



#endif