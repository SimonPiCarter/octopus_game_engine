#ifndef __STATE__
#define __STATE__

#include <vector>
#include "Handle.hh"

namespace octopus
{
class Entity;

/// @brief represent the whole world
/// @warning a State must only be modified through step apply and revert!
class State
{
public:
	State();
	~State();

	Entity *getEntity(Handle const &handle_p);
	Entity const *getEntity(Handle const &handle_p) const;

	/// @brief add entity to state (keep ownership)
	/// @warning handle will be modified!
	void addEntity(Entity * ent_p);

	std::vector<Entity *> const &getEntities() const;

private:
	/// @brief vector of all entities
	std::vector<Entity *> _entities;
};

} // namespace octopus

#endif
