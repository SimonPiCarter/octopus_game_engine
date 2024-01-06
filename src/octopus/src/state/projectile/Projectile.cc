#include "Projectile.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "logger/Logger.hh"

namespace octopus
{

Vector getMove(Vector const &source_p, Vector const &target_p, Fixed const speed_p)
{
	Vector move_l = target_p - source_p;
	Fixed length_l = length(move_l);
	if(length_l < speed_p)
	{
		return move_l;
	}
	move_l *= speed_p/length_l;

	return move_l;
}

void tickProjectile(Step &step_p, Projectile const &proj_p, State const &state_p)
{
	if(proj_p._done)
	{
		return;
	}
	static Fixed range_check(Fixed::OneAsLong()/10, true);

	Vector target_l = proj_p._posTarget;
	if(state_p.isEntityAlive(proj_p._target))
	{
		target_l = state_p.getLoseEntity(proj_p._target.index)->_pos;
	}
	// if in range generate impact steps
	if(square_length(proj_p._pos-target_l) < range_check*range_check)
	{
		// this cannot be parallelized
		AttackModifierData attackModData_l {proj_p._source, proj_p._target,
			target_l,
			proj_p._sourceTeam,
			proj_p._baseDamage,
			proj_p._bonusDamage
		};
		newAttackSteppable(proj_p._generator, step_p, attackModData_l, state_p);
		step_p.getProjectileMoveStep().setOver(proj_p._index);
	}
	// else move to target
	else
	{
		step_p.getProjectileMoveStep().setMove(proj_p._index, getMove(proj_p._pos, target_l, proj_p._speed));
	}
}

void ProjectileContainer::markDone(uint32_t index_p)
{
	_projectiles[index_p]._done = true;
	_freeIdx.push_back(index_p);
}

void ProjectileContainer::unmarkDone(uint32_t index_p)
{
	_projectiles[index_p]._done = false;
	if(_freeIdx.back() != index_p)
	{
		throw std::logic_error("Error while unmarking projectile as done, free idx does not match the one being undone");
	}
	_freeIdx.pop_back();
}

} // namespace octopus
