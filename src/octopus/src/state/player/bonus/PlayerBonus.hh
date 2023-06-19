#ifndef __PlayerBonus__
#define __PlayerBonus__

#include "state/entity/buff/Buff.hh"

#include <string>
#include <unordered_map>

namespace octopus
{
    struct CostBonus
    {
        std::unordered_map<std::string, Buff> _resourceModifier;
    };

    struct PlayerBonus
    {
        /// @brief time production modifiers per model id
        std::unordered_map<std::string, Buff> _bonusTimeProduction;

        /// @brief resource cost modifiers per model id
        std::unordered_map<std::string, CostBonus> _bonusCost;
    };
} // namespace octopus


#endif
