#ifndef __FASUtilsRandomize__
#define __FASUtilsRandomize__

#include <string>

namespace octopus
{
    class RandomGenerator;
} // namespace octopus

namespace fas
{

std::string genModelName(octopus::RandomGenerator &gen_p);

} // fas
#endif
