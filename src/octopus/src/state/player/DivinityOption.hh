#ifndef __DivinityOption__
#define __DivinityOption__

#include <set>
#include "state/DivinityType.hh"

namespace octopus
{
    struct DivinityOption
    {
        bool _lvlUp;
        std::set<DivinityType> _types;
    };
} // namespace octopus


#endif