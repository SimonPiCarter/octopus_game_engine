#ifndef __STATE__
#define __STATE__

#include <vector>
#include "Handle.hh"
#include "utils/DynamicBitset.hh"

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

	std::vector<std::vector<DynamicBitset> > const & getGrid() const;
	std::vector<std::vector<DynamicBitset> > & getGrid();

	unsigned long getGridSize() const;
	unsigned long getGridBitSize() const;

	unsigned long const _id;
private:
	/// @brief vector of all entities
	std::vector<Entity *> _entities;

	/// @brief vector of all commandables
	std::vector<Commandable *> _commandables;

	/// @brief grid for position indexing
	std::vector<std::vector<DynamicBitset> > _grid;
	unsigned long const _gridSize;
	unsigned long const _gridBitSize;
};

Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p);

void updateGrid(State &state_p, Entity const *ent_p, bool set_p);

} // namespace octopus

#endif
