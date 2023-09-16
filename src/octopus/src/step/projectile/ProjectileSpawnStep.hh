#ifndef __ProjectileSpawnStep__
#define __ProjectileSpawnStep__

#include "state/projectile/Projectile.hh"
#include "step/Steppable.hh"
#include <vector>

namespace octopus
{

/// @brief
class ProjectileSpawnStep : public Steppable
{
public:
	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	void addProjectile(ProjectileContainer const &container_p, Projectile &&projectile_p);

	std::vector<Projectile> const &getToBeSpawned() const;
private:
	std::vector<Projectile> _toBeSpawned;
};

} // namespace octopus


#endif // __ProjectileSpawnStep__