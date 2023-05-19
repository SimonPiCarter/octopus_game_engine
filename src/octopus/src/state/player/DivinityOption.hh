#ifndef __DivinityOption__
#define __DivinityOption__

#include <set>
#include <string>

namespace octopus
{
    struct DivinityOption
    {
        bool _lvlUp;
        std::set<std::string> _types;
    };
} // namespace octopus


#endif