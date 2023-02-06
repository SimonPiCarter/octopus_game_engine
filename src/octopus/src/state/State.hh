#ifndef __STATE__
#define __STATE__

#include <vector>
#include "Handle.hh"
#include "ResourceType.hh"
#include "graph/Grid.hh"
#include "utils/DynamicBitset.hh"

namespace octopus
{
class Entity;
class Commandable;
class Player;

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

	std::vector<Player *> &getPlayers();
	std::vector<Player *> const &getPlayers() const;

	Player *getPlayer(unsigned long player_p);
	Player const *getPlayer(unsigned long player_p) const;

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
	State(State const &state_p) = delete;

	/// @brief vector of all entities
	std::vector<Entity *> _entities;

	/// @brief vector of all commandables
	std::vector<Commandable *> _commandables;

	/// @brief vector of players
	std::vector<Player *> _players;

	/// @brief list of triggers (one shot and on each)
	std::vector<OneShotTrigger *> _oneShotTriggers;
	std::vector<OnEachTrigger *> _onEachTriggers;

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
Entity const * lookUpDeposit(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p);
Entity const * lookUpNewResource(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p);

bool checkResource(State const &state_p, unsigned long player_p, std::map<ResourceType, double> const & cost_p);

void updateGrid(State &state_p, Entity const *ent_p, bool set_p);

/// @brief check if the grid elements corresponding to the entity
/// are free
bool checkGrid(State const &state_p, Entity const *ent_p);

std::list<Vector> computePath(State const & state_p, Handle const &handle_p, Vector const &target_p, std::list<Entity const *> const &ignored_p = {});

} // namespace octopus

#endif
