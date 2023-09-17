#include "SelfDamageModifier.hh"

#include "command/flying/CommandEffectOverTime.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void SelfDamageModifier::newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool) const
{
	if(state_p.isEntityAlive(data_p.source))
	{
		Entity const & source_l = *state_p.getLoseEntity(data_p.source.index);
		Fixed curHp_l = source_l._hp + step_p.getHpChange(source_l._handle);
		Fixed maxHp_l = source_l.getHpMax();
		Fixed dmg_l = _damage + (source_l._hp + step_p.getHpChange(source_l._handle)) * _ratioCurHp + source_l.getHpMax() * _ratioMaxHp;
		// if cannot kill we set hp to 1 at minimum
		if(!_canKill && dmg_l >= curHp_l)
		{
			dmg_l = curHp_l-1.;
		}
		step_p.addSteppable(new EntityHitPointChangeStep(source_l._handle, -dmg_l, curHp_l, maxHp_l));
	}
}

} // namespace octopus
