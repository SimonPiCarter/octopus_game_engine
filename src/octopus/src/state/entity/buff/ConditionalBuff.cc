
#include "ConditionalBuff.hh"

#include "state/entity/Entity.hh"

namespace octopus
{

bool applies(Entity const &ent_p, ConditionMinLife const &cond_p)
{
    return ent_p._hp / ent_p.getHpMax(false) >= cond_p.minRatio;
}

bool applies(Entity const &ent_p, ConditionMaxLife const &cond_p)
{
    return ent_p._hp / ent_p.getHpMax(false) <= cond_p.maxRatio;
}

bool ConditionalBuff::isApplying(Entity const &ent_p) const
{
    bool applies_l = true;
    for(Condition const &cond_l : _conditions)
    {
        std::visit([&](auto &&arg) { applies_l &= applies(ent_p, arg); }, cond_l);
    }

    return applies_l;
}

} // namespace octopus
