#ifndef __SelfDamageModifier__
#define __SelfDamageModifier__

#include "utils/Fixed.hh"
#include <vector>

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;

/// @brief this class is aimed at self damaging on attack
class SelfDamageModifier
{
public:
    SelfDamageModifier(double damage_p, double ratioCurHp_p, double ratioMaxHp_p, bool canKill_p)
        : _damage(damage_p), _ratioCurHp(ratioCurHp_p), _ratioMaxHp(ratioMaxHp_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool disableMainAttack_p=false) const;

    double _damage;
    double _ratioCurHp;
    double _ratioMaxHp;
    bool _canKill;
};

} // namespace octopus


#endif
