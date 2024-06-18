#ifndef __Projectile__
#define __Projectile__

#include <list>

#include "state/Handle.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "utils/Vector.hh"
#include "utils/Fixed.hh"

namespace octopus
{

struct EntityModel;
class State;
class Step;

/// @brief barebone data for projectiles
struct Projectile
{
	/// @brief index of this projectile in the container
	uint32_t _index {0};
	/// @brief indicate if projectile is done (and free for reuse)
	bool _done {false};
	/// @brief position of the projectile
	Vector _pos;
	/// @brief position of the target of the projectile
	Vector _posTarget;
	/// @brief target of the projectile (can be invalid in case of no target or if target died)
	/// in that case fall back to _posTarget
	Handle _target;
	/// @brief source of the projectile
	Handle _source;
	/// @brief the model of the source
	EntityModel const * _sourceModel {nullptr};
	/// @brief the team of the source of the projectile
	unsigned long _sourceTeam;
	/// @brief projectile step speed
	Fixed _speed;
	/// @brief damage with NO bonuses
	Fixed _baseDamage;
	/// @brief total damage with bonuses
	Fixed _bonusDamage;
	/// @brief step generator when projectile hit
	AttackModifier _generator;
};

/// @brief class responsible for storing and registering every Projectile
class ProjectileContainer
{
public:
	std::vector<Projectile> & getProjectiles() {return _projectiles; }
	std::vector<Projectile> const & getProjectiles() const {return _projectiles; }

	/// @brief mark the projectile as done and register it as free idx
	void markDone(uint32_t index_p);
	/// @brief mark the projectile as NOT done and remove it from free idx
	void unmarkDone(uint32_t index_p);

	std::list<uint32_t> const &getFreeIdx() const { return _freeIdx; }
	std::list<uint32_t> &getFreeIdx() { return _freeIdx; }

private:
	std::vector<Projectile> _projectiles;

	/// @brief list of free indexes in the vector
	std::list<uint32_t> _freeIdx;
};

/// @brief Handle the basic ticking for a projectile
/// - if projectile is close to target it will generate steps of impact and
/// mark itself as finished
/// - otherwise it will close on target (entity or position)
void tickProjectile(Step &step_p, Projectile const &proj_p, State const &state_p);

} // namespace octopus


#endif // __Projectile__