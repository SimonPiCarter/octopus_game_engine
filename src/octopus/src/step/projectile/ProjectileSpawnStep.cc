#include "ProjectileSpawnStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void ProjectileSpawnStep::apply(State &state_p) const
{
	Logger::getDebug() << "ProjectileSpawnStep :: apply "<<std::endl;
	ProjectileContainer & container_l = state_p.getProjectileContainer();

	for(size_t i = 0 ; i < _toBeSpawned.size() ; ++ i)
	{
		if(_toBeSpawned[i]._index < container_l.getProjectiles().size())
		{
			container_l.getProjectiles()[i] = _toBeSpawned[i];
		}
		else
		{
			if(_toBeSpawned[i]._index != container_l.getProjectiles().size())
			{
				throw std::logic_error("Invalid index for projectile for state "+std::to_string(state_p._id));
			}
			container_l.getProjectiles().emplace_back(_toBeSpawned[i]);
		}
		// release free indexes
		if(container_l.getFreeIdx().front() == _toBeSpawned[i]._index)
		{
			container_l.getFreeIdx().pop_front();
		}
	}
}

void ProjectileSpawnStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "ProjectileSpawnStep :: revert "<<std::endl;
	ProjectileContainer & container_l = state_p.getProjectileContainer();

	for(size_t i = _toBeSpawned.size() ; i > 0 ; -- i)
	{
		// disable projectile
		container_l.getProjectiles()[i-1]._done = true;
		// add it as free
		container_l.getFreeIdx().push_front(i);
	}
}

bool ProjectileSpawnStep::isNoOp() const
{
	return _toBeSpawned.empty();
}

size_t get(std::list<size_t> const &freeIdx_p, size_t idx_p)
{
	auto &&it_l = freeIdx_p.begin();
	for(size_t count_l = 0 ; count_l < idx_p ; ++ count_l)
	{
		++it_l;
	}
	return *it_l;
}

void ProjectileSpawnStep::addProjectile(ProjectileContainer const &container_p, Projectile &&projectile_p)
{
	std::list<size_t> const &freeIdx_l = container_p.getFreeIdx();
	std::vector<Projectile> const &proj_l = container_p.getProjectiles();

	if(_toBeSpawned.size() < freeIdx_l.size())
	{
		projectile_p._index = get(freeIdx_l, _toBeSpawned.size());
	}
	else
	{
		projectile_p._index = proj_l.size() + _toBeSpawned.size() - freeIdx_l.size();
	}

	_toBeSpawned.emplace_back(projectile_p);
}

std::vector<Projectile> const &ProjectileSpawnStep::getToBeSpawned() const
{
	return _toBeSpawned;
}

} // namespace octopus

