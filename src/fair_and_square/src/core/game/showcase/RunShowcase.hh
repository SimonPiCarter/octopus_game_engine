#ifndef __RunGame__
#define __RunGame__

#include <cstddef>

namespace cuttlefish
{
    class Window;

} // namespace cuttlefish

namespace fas
{
    void runAoeShowcase(cuttlefish::Window &window_p);
    void runChainingShowcase(cuttlefish::Window &window_p);
    void runDotShowcase(cuttlefish::Window &window_p);
    void runLifestealShowcase(cuttlefish::Window &window_p);
} // namespace fas

#endif
