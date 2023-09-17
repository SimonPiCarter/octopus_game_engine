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
    ChainingOverTime(Handle handle_p, unsigned long tickRate_p, Fixed dmg_p,
                    Handle ent_p, unsigned long nbOfOccurence_p, Fixed ratio_p,
                    Fixed range_p, unsigned long team_p, std::list<Handle> const &oldTargets_p = {})
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
    Fixed const _dmg;
    /// @brief the handle of entity to deal damage to
    Handle const _ent;

    unsigned long const _nbOfOccurence;
    Fixed const _ratio;
    Fixed const _range;
    unsigned long const _team;

    std::list<Handle> _oldTargets;
};

void ChainingOverTime::applyEffect(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
    // find new target
    Entity const *target_l = nullptr;

	// effect stops if entity died
	if(!state_p.isEntityAlive(_ent))
	{
		return;
	}

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

    Fixed maxHp_l = target_l->getHpMax();
    Fixed curHp_l = target_l->_hp + step_p.getHpChange(_ent);
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

void ChainingModifier::newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p) const
{
    unsigned long team_l = data_p.team;

    Handle idx_l = state_p.getFlyingCommandHandle(step_p.getFlyingCommandSpawned());
    step_p.addSteppable(new FlyingCommandSpawnStep(
        new ChainingOverTime(idx_l, _delay, data_p.baseDamage*_ratio, data_p.target, _nbOfTicks, _ratio, _range, team_l, {data_p.target})));

    if(!disableMainAttack_p)
    {
        applyMainAttack(step_p, data_p, state_p);
    }
}

} // namespace octopus
