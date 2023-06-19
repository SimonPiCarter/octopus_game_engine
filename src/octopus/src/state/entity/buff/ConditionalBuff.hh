#ifndef __ConditionalBuff__
#define __ConditionalBuff__

#include "Buff.hh"
#include <variant>
#include <vector>

namespace octopus
{

/// @brief This condition is true as long as entity has a minimal amount of life (in 0...1)
struct ConditionMinLife
{
    Fixed minRatio;
};

/// @brief This condition is true as long as entity has a maximal amount of life (in 0...1)
struct ConditionMaxLife
{
    Fixed maxRatio;
};

using Condition = std::variant<ConditionMinLife, ConditionMaxLife>;

struct ConditionalBuff : public TyppedBuff
{
	/// @brief check if the buff apply to the entity
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	bool isApplying(Entity const &ent_p) const;

    /// @brief all condition must be met for the buff to apply
    std::vector<Condition> _conditions;

    /// @brief this is only used for weak identification purpose (no unicity check in entities)
    std::string _id;
};

}

#endif
