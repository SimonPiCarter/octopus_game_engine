#ifndef __STATE__
#define __STATE__

#include <vector>
#include "Handle.hh"
#include "ResourceType.hh"
#include "graph/Grid.hh"
#include "utils/DynamicBitset.hh"

namespace octopus
{
class Commandable;
class Entity;
struct ListenerData;
class Player;
class Trigger;
struct TriggerData;
struct TyppedBuff;

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

	std::vector<std::vector<AbstractBitset *> > const & getGrid() const;
	std::vector<std::vector<AbstractBitset *> > & getGrid();

	///
	/// Trigger and Listener data
	///
	ListenerData * getListenerData(Handle const &handleTrigger_p, Handle const &handleListener_p);
	const ListenerData * getListenerData(Handle const &handleTrigger_p, Handle const &handleListener_p) const;

	TriggerData * getTriggerData(Handle const &handleTrigger_p);
	const TriggerData * getTriggerData(Handle const &handleTrigger_p) const;

	std::vector<Trigger const *> &getTriggers() { return _triggers; }
	std::vector<Trigger const *> const &getTriggers() const { return _triggers; }
	std::vector<TriggerData *> &getTriggersData() { return _triggersData; }
	std::vector<TriggerData *> const &getTriggersData() const { return _triggersData; }

	unsigned long getGridSize() const;

	Grid &getPathGrid();
	Grid const &getPathGrid() const;

	/// @brief increment path grid status
	/// this status is used to know if a path is deprecated
	/// and should be recomputed or not
	void incrementPathGridStatus();
	unsigned long getPathGridStatus() const { return _pathGridStatus; }

	/// @brief get grid index from world position
	/// @note basically divide by grid point size and floor value
	long getGridIndex(double idx_p) const;

	std::list<Vector> &getTemplePosition() { return _templePosition; }
	const std::list<Vector> &getTemplePosition() const { return _templePosition; }

	unsigned long const _id;
private:
	State(State const &state_p) = delete;

	/// @brief vector of all entities
	std::vector<Entity *> _entities;

	/// @brief vector of all commandables
	std::vector<Commandable *> _commandables;

	/// @brief vector of players
	std::vector<Player *> _players;

	/// @brief list of triggers ("one shot" and "on each")
	std::vector<Trigger const *> _triggers;

	/// @brief trigger data (multiple trigger for one shot but only one allowed for on each)
	std::vector<TriggerData *> _triggersData;

	/// @brief grid for position indexing
	std::vector<std::vector<AbstractBitset *> > _grid;
	unsigned long const _gridSize;
	unsigned long const _gridPointSize;

	/// @brief list of position where temple are allowed
	std::list<Vector> _templePosition;

	/// @brief grid for pathing purpose
	Grid _pathGrid;
	/// @brief used to know if grid has changed
	unsigned long _pathGridStatus {0};
};

Entity const * lookUpNewBuffTarget(State const &state_p, Handle const &sourceHandle_p, double range_p, TyppedBuff const &buff_p);
Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p);
Entity const * lookUpDeposit(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p);
Entity const * lookUpNewResource(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p);

/// @brief Check if the player has enough resource in the given state
/// @param state_p state in which we want to check resources
/// @param player_p the player
/// @param cost_p the cost
/// @param spent_p the resources already spent (not applied yet on the state)
/// @return true iif player has enough resource
bool checkResource(State const &state_p, unsigned long player_p, std::map<ResourceType, double> const & cost_p, std::map<ResourceType, double> const & spent_p);

void updateGrid(State &state_p, Entity const *ent_p, bool set_p);

/// @brief check if the grid elements corresponding to the entity
/// are free
bool checkGrid(State const &state_p, Entity const *ent_p, bool ignoreAbandonedTemples_p);

std::list<Vector> computePath(State const & state_p, Handle const &handle_p, Vector const &target_p, std::list<Entity const *> const &ignored_p = {});

} // namespace octopus

#endif
