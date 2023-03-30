#include "ChainingModifier.hh"

#include "command/flying/CommandEffectOverTime.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{


/// @brief This class is a CommandEffectOverTime
/// aimed at reducing a resource, 1 resource per second
/// if resource if at 0 then deal 10 damage per second
class ChainingOverTime : public CommandEffectOverTime
{
public:
    ChainingOverTime(Handle handle_p, unsigned long tickRate_p, double dmg_p,
                    Handle ent_p, unsigned long nbOfOccurence_p, double ratio_p,
                    double range_p, unsigned long team_p, std::list<Handle> const &oldTargets_p = {})
        : CommandEffectOverTime(handle_p, tickRate_p, 1)
        , _dmg(dmg_p)
        , _ent(ent_p)
        , _nbOfOccurence(nbOfOccurence_p)
        , _ratio(ratio_p)
        , _range(range_p)
        , _team(team_p)
        , _oldTargets(oldTargets_p)
    {}

	virtual void applyEffect(Step & step_p, State const &state_p, CommandData const * , PathManager &) const override;
protected:
    /// @brief damage per tick
    double const _dmg;
    /// @brief the handle of entity to deal damage to
    Handle const _ent;

    unsigned long const _nbOfOccurence;
    double const _ratio;
    double const _range;
    unsigned long const _team;

    std::list<Handle> _oldTargets;
};

void ChainingOverTime::applyEffect(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
    // find new target
    Entity const *target_l = nullptr;

    TargetPanel panel_l(lookUpNewTargets(state_p, _ent, _range, false));
    for(Entity const * subTarget_l : panel_l.units)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != _team && std::find(_oldTargets.begin(), _oldTargets.end(), subTarget_l->_handle) == _oldTargets.end())
        {
            target_l = subTarget_l;
            break;
        }
    }

    if(!target_l)
    {
        return;
    }

    double maxHp_l = target_l->getHpMax();
    double curHp_l = target_l->_hp + step_p.getHpChange(_ent);
    step_p.addSteppable(new EntityHitPointChangeStep(target_l->_handle, -_dmg, curHp_l, maxHp_l));

    if(_nbOfOccurence > 1)
    {
        std::list<Handle> oldTargets_l = _oldTargets;
        oldTargets_l.push_back(target_l->_handle);
        Handle idx_l = state_p.getFlyingCommandHandle(step_p.getFlyingCommandSpawned());
        step_p.addSteppable(new FlyingCommandSpawnStep(
            new ChainingOverTime(idx_l, _tickRate, _dmg*_ratio, target_l->_handle, _nbOfOccurence-1, _ratio, _range, _team, oldTargets_l)));
    }
}

std::vector<Steppable *> ChainingModifier::newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const
{
    std::vector<Steppable *> vec_l;

    Player const *player_l = state_p.getPlayer(ent_p._player);
    unsigned long team_l = player_l->_team;

    Handle idx_l = state_p.getFlyingCommandHandle(step_p.getFlyingCommandSpawned());
    vec_l.push_back(new FlyingCommandSpawnStep(
        new ChainingOverTime(idx_l, _delay, ent_p.getDamageNoBonus()*_ratio, target_p._handle, _nbOfTicks, _ratio, _range, team_l)));

	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
    double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
    double maxHp_l = target_p.getHpMax();
    vec_l.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l));

    return vec_l;
}

} // namespace octopus
