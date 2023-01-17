#ifndef __STATE__
#define __STATE__

#include <vector>
#include "Handle.hh"
#include "graph/Grid.hh"
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

	Grid &getPathGrid();
	Grid const &getPathGrid() const;

	/// @brief increment path grid status
	/// this status is used to know if a path is deprecated
	/// and should be recomputed or not
	void incrementPathGridStatus();
	unsigned long getPathGridStatus() const { return _pathGridStatus; }

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

	/// @brief grid for pathing purpose
	Grid _pathGrid;
	/// @brief used to know if grid has changed
	unsigned long _pathGridStatus {0};
};

Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p);

void updateGrid(State &state_p, Entity const *ent_p, bool set_p);

std::list<Vector> computePath(State const & state_p, Handle const &handle_p, Vector const &target_p);

} // namespace octopus

#endif
