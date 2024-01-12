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
	for(uint32_t i = 0 ; i < _move.size() ; ++ i)
	{
		if(!is_zero(_move[i]))
		{
			container_l.getProjectiles()[i]._pos += _move[i];
		}
	}
	for(uint32_t i = 0 ; i < _over.size() ; ++ i)
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
	for(uint32_t i = _move.size() ; i > 0 ; -- i)
	{
		if(!is_zero(_move[i]))
		{
			container_l.getProjectiles()[i-1]._pos -= _move[i];
		}
	}
	for(uint32_t i = _move.size() ; i > 0 ; -- i)
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

void ProjectileMoveStep::setOver(uint32_t idx_p)
{
	if(idx_p >= _over.size())
	{
		_over.resize(idx_p+1, false);
	}
	_over[idx_p] = true;
}

void ProjectileMoveStep::setMove(uint32_t idx_p, Vector &&vec_p)
{
	if(idx_p >= _move.size())
	{
		_move.resize(idx_p+1);
	}
	_move[idx_p] = vec_p;
}

void ProjectileMoveStep::merge(ProjectileMoveStep &&other_p)
{
	if(_move.empty())
	{
		_move = std::move(other_p._move);
	}
	// merge by suming all moves
	else
	{
		if(other_p._move.size() > _move.size())
		{
			_move.resize(other_p._move.size());
		}
		for(size_t i = 0 ; i < other_p._move.size() ; ++ i)
		{
			_move[i] += other_p._move[i];
		}
	}
	if(_over.empty())
	{
		_over = std::move(other_p._over);
	}
	// merge by ORing all over boolean
	else
	{
		if(other_p._over.size() > _over.size())
		{
			_over.resize(other_p._over.size(), false);
		}
		for(size_t i = 0 ; i < other_p._over.size() ; ++ i)
		{
			_over[i] = _over[i] || other_p._over[i];
		}
	}

}

} // namespace octopus

