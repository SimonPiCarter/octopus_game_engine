#include "LifeStealModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void LifeStealModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool disableMainAttack_p) const
{
    // damage
	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
    if(!disableMainAttack_p)
    {
        double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
        double maxHp_l = target_p.getHpMax();
        vec_r.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l));
    }
    // lifesteal
    {
        double curHp_l = ent_p._hp + step_p.getHpChange(ent_p._handle);
        double maxHp_l = ent_p.getHpMax();
	    vec_r.push_back(new EntityHitPointChangeStep(ent_p._handle, -dmg_l*_ratio, curHp_l, maxHp_l));
    }
}

} // namespace octopus
