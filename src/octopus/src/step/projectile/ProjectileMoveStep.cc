#include "ProjectileMoveStep.hh"

#include "state/projectile/Projectile.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void ProjectileMoveStep::apply(State &state_p) const
{
	Logger::getDebug() << "ProjectileMoveStep :: apply "<<std::endl;
	ProjectileContainer & container_l = state_p.getProjectileContainer();
	for(size_t i = 0 ; i < _move.size() ; ++ i)
	{
		if(!is_zero(_move[i]))
		{
			container_l.getProjectiles()[i]._pos += _move[i];
		}
	}
	for(size_t i = 0 ; i < _over.size() ; ++ i)
	{
		if(_over[i])
		{
			container_l.markDone(i);
		}
	}
}

void ProjectileMoveStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "ProjectileMoveStep :: revert "<<std::endl;
	ProjectileContainer & container_l = state_p.getProjectileContainer();
	for(size_t i = _move.size() ; i > 0 ; -- i)
	{
		if(!is_zero(_move[i]))
		{
			container_l.getProjectiles()[i-1]._pos -= _move[i];
		}
	}
	for(size_t i = _move.size() ; i > 0 ; -- i)
	{
		if(_over[i-1])
		{
			container_l.unmarkDone(i-1);
		}
	}
}

bool ProjectileMoveStep::isNoOp() const
{
	return false;
}

void ProjectileMoveStep::setOver(size_t idx_p)
{
	if(idx_p >= _over.size())
	{
		_over.resize(idx_p+1, false);
	}
	_over[idx_p] = true;
}

void ProjectileMoveStep::setMove(size_t idx_p, Vector &&vec_p)
{
	if(idx_p >= _move.size())
	{
		_move.resize(idx_p+1);
	}
	_move[idx_p] = vec_p;
}

} // namespace octopus

