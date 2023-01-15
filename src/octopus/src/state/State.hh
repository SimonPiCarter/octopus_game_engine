#ifndef __STATE__
#define __STATE__

#include <vector>
#include "Handle.hh"

namespace octopus
{
class Entity;
class Commandable;

/// @brief represent the whole world
/// @warning a State must only be modified through step apply and revert!
class State
{
public:
	State();
	explicit State(unsigned long id_p);
	~State();

	bool hasEntity(Handle const &handle_p) const;
	Entity *getEntity(Handle const &handle_p);
	Entity const *getEntity(Handle const &handle_p) const;

	bool hasCommandable(Handle const &handle_p) const;
	Commandable *getCommandable(Handle const &handle_p);
	Commandable const *getCommandable(Handle const &handle_p) const;

	/// @brief add entity to state (keep ownership)
	/// @warning handle will be modified!
	Handle addEntity(Entity * ent_p);

	std::vector<Entity *> const &getEntities() const;
	std::vector<Commandable *> const &getCommandables() const;

	unsigned long const _id;
private:
	/// @brief vector of all entities
	std::vector<Entity *> _entities;

	/// @brief vector of all commandables
	std::vector<Commandable *> _commandables;
};

Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p);

} // namespace octopus

#endif
