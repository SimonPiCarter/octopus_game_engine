#ifndef __STATE__
#define __STATE__

#include <map>
#include <vector>
#include "Handle.hh"
#include "graph/Grid.hh"
#include "utils/DynamicBitset.hh"
#include "utils/Box.hh"
#include "step/ConstraintPosition.hh"

#include "projectile/Projectile.hh"
#include "vision/VisionHandler.hh"

namespace octopus
{
class Commandable;
class CommandData;
class Entity;
struct ListenerData;
class Player;
class Trigger;
struct TriggerData;
struct TimedBuff;
class FlyingCommand;

struct FlyingCommandBundle
{
	FlyingCommandBundle(CommandData * data_p, FlyingCommand * cmd_p);
	CommandData * _data {nullptr};
	FlyingCommand * _cmd {nullptr};
	Handle const getHandle() const;
};

typedef std::map<Handle, FlyingCommandBundle> FlyingCommandSet;

/// @brief represent the whole world
/// @warning a State must only be modified through step apply and revert!
class State
{
public:
	State();
	/// @brief create a state with the given id and each grid node with the given size
	State(unsigned long id_p, unsigned long gridSize_p, unsigned long gridPointSize_p);
	~State();

	bool isEntityAlive(Handle const &handle_p) const;
	bool hasEntity(Handle const &handle_p) const;
	Entity *getEntity(Handle const &handle_p);
	Entity const *getEntity(Handle const &handle_p) const;
	/// @brief Get entity without checking revision
	/// @remark this should only be called in very internal calls!
	Entity const *getLoseEntity(unsigned long handle_p) const;

	bool hasCommandable(Handle const &handle_p) const;
	Commandable *getCommandable(Handle const &handle_p);
	Commandable const *getCommandable(Handle const &handle_p) const;

	/// @brief add entity to state (keep ownership)
	/// @warning handle will be modified!
	Handle const &addEntity(Entity * ent_p);

	/// @brief register a free handle (entity is not alive anymore)
	void addFreeHandle(Handle const &handle_p);
	/// @brief remove the last added free handle
	void popBackFreeHandle();
	/// @brief get the list of free handles
	std::list<Handle> const &getFreeHandles() const;

	/// @brief initialize the queued free handles with the desired size
	void initializeQueueFreeHandles(size_t size_p);
	/// @brief unfold one stack of the queued free handles
	/// @note this should be called every time a step is applied (in the ticking step)
	void unfoldQueuedFreeHandles();
	/// @brief restore a list of handled back at the front of the queue and clear them from the
	/// free handles list (they should all be at the back of the list)
	void refoldQueuedFreeHandles(std::list<Handle> const &handles_p);
	/// @brief return the front queued handles (used to store some data for eventual restore)
	std::list<Handle> const &getFrontQueuedHandles() const;

	std::vector<Entity *> &getEntities();
	std::vector<Entity *> const &getEntities() const;
	std::vector<Commandable *> const &getCommandables() const;

	FlyingCommandSet & getFlyingCommands();
	const FlyingCommandSet & getFlyingCommands() const;

	/// @brief return the next available handle
	/// @param spawning_p the number of available handles that will be used (need to iterate further the first available handle if > 0)
	Handle getFlyingCommandHandle(unsigned long spawning_p) const;

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
	const std::vector<Trigger const *> &getTriggers() const { return _triggers; }
	std::vector<TriggerData *> &getTriggersData() { return _triggersData; }
	const std::vector<TriggerData *> &getTriggersData() const { return _triggersData; }

	///
	/// constraint position library
	///
	ConstraintPositionLibrary &getConstraintPositionLibrary() { return _constraintPositionLibrary; }
	const ConstraintPositionLibrary &getConstraintPositionLibrary() const { return _constraintPositionLibrary; }

	unsigned long getGridSize() const;
	unsigned long getWorldSize() const;

	Grid &getPathGrid();
	Grid const &getPathGrid() const;

	/// @brief increment path grid status
	/// this status is used to know if a path is deprecated
	/// and should be recomputed or not
	void incrementPathGridStatus();
	unsigned long getPathGridStatus() const { return _pathGridStatus; }

	/// @brief get grid index from world position
	/// @note basically divide by grid point size and floor value
	long getGridIndex(Fixed idx_p) const;

	std::list<Vector> &getTemplePosition() { return _templePosition; }
	const std::list<Vector> &getTemplePosition() const { return _templePosition; }

	VisionHandler &getVisionHandler() { return _visionHandler; }
	const VisionHandler &getVisionHandler() const { return _visionHandler; }

	unsigned long const _id;

	/// @brief set the over boolean
	void setIsOver(bool over_p);
	/// @brief set the hasWinner boolean
	void setHasWinner(bool hasWinner_p);
	/// @brief set the winning team
	void setWinningTeam(unsigned long team_p);

	/// @brief check if the state represent a game over
	bool isOver() const;
	/// @brief check if the state represent a draw
	bool hasWinningTeam() const;
	/// @brief return the winning team
	unsigned long getWinningTeam() const;

	/// @brief add step applied
	void addStepApplied();
	/// @brief remove step applied
	void removeStepApplied();
	/// @brief get the number of step applied
	unsigned long getStepApplied() const;

	/// @brief getter for projectile container
	ProjectileContainer &getProjectileContainer();
	/// @brief getter for projectile container
	ProjectileContainer const &getProjectileContainer() const;
private:
	State(State const &state_p) = delete;

	/// @brief vector of all entities
	std::vector<Entity *> _entities;

	/// @brief list of free handles
	std::list<Handle> _freeHandles;

	/// @brief queued list of free handles
	/// @note  this list enable to queue free handle
	/// the front stack is added to the free handles every step applied
	/// and one new stack is added at the back. This is important to
	/// make sure that handles are not reused before all reference to them
	/// are cleared.
	/// @warning the queue size cannot be 0 and represent how many steps
	/// are applied before a free handle can be reused.
	std::list<std::list<Handle>> _queuedfreeHandles = {{}};

	/// @brief vector of all commandables
	std::vector<Commandable *> _commandables;

	/// @brief vector of all flying command to be run
	FlyingCommandSet _flyingCommands;

	/// @brief vector of players
	std::vector<Player *> _players;

	/// @brief list of triggers ("one shot" and "on each")
	std::vector<Trigger const *> _triggers;

	/// @brief trigger data (multiple trigger for one shot but only one allowed for on each)
	std::vector<TriggerData *> _triggersData;

	/// @brief library of constraints on position applying in this state
	ConstraintPositionLibrary _constraintPositionLibrary;

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

	/// @brief the vision handler for this step
	VisionHandler _visionHandler;

	/// @brief true if the game is over
	bool _isOver {false};
	/// @brief true if there is a winning team
	bool _hasWinner {false};
	/// @brief the idx of the winning team
	unsigned long _winningTeam {0};

	/// @brief number of step applied
	unsigned long _stepApplied {0};

	/// @brief projectile container
	ProjectileContainer _projectileContainer;
};

/// @brief struct to represent a panel of targets within a range
struct TargetPanel
{
	std::list<Entity const *> units;
	std::list<Entity const *> buildings;
	Fixed matchDistance {0.};
};

/// @brief look for new targets in a matching range
/// @return all targets within range
TargetPanel lookUpNewTargets(State const &state_p, Handle const &sourceHandle_p, Fixed matchDistance_p, bool filterTeam_p=true);
/// @brief look for new targets in a matching range (do not use the source handle to fetch the entity)
/// @return all targets within range
TargetPanel lookUpNewTargets(State const &state_p, unsigned long sourceIndex_p, Vector const &pos_p, unsigned long team_p, Fixed matchDistance_p, bool filterTeam_p=true);

Entity const * lookUpNewBuffTarget(State const &state_p, Handle const &sourceHandle_p, Fixed range_p, TimedBuff const &buff_p);
Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p, Fixed matchDistance_p = 5., bool healing_p=false);
Entity const * lookUpDeposit(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p);
Entity const * lookUpNewResource(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p);

/// @brief Check if there are non static behind (look up) the given entity
/// @param state_p the state
/// @param handle_p the handle of the entity to be checked
/// @param width_p by how many cases we check on left and right
/// @param height_p by how many cases we check up
/// @return true if one entity is found
bool lookUpNonStaticBehind(State const &state_p, Handle const &handle_p, unsigned long width_p, unsigned long height_p);
bool lookUpNonStaticBehind(State const &state_p, Fixed x, Fixed y, unsigned long width_p, unsigned long height_p);

/// @brief Check if the player has enough resource in the given state
/// @param state_p state in which we want to check resources
/// @param player_p the player
/// @param cost_p the cost
/// @param spent_p the resources already spent (not applied yet on the state)
/// @return true iif player has enough resource
bool checkResource(State const &state_p, unsigned long player_p, std::map<std::string, Fixed> const & cost_p, std::map<std::string, Fixed> const & spent_p);

/// @brief update positional info based on the entity and the set parameter
void updateGrid(State &state_p, Entity const *ent_p, bool set_p);

/// @brief update the current vision grid based on the entity and the set parameter
/// the global vision is used to know wether a node is being seen at this point
void updateVisionGrid(State &state_p, Entity const *ent_p, bool set_p);

/// @brief update the global vision grid based on the entity and the set parameter
/// the global vision is used to know wether a node has been explored at this point
/// it is like the current vision but not reset when moving or dying
void updateExplorationGrid(State &state_p, Entity const *ent_p, bool set_p);

/// @brief update the current vision grid based on the entity and the movements
/// the global vision is used to know wether a node is being seen at this point
void updateVisionGridFromMovement(State &state_p, Entity const *ent_p, long dx, long dy);

/// @brief update the global vision grid based on the entity and he movements
/// the global vision is used to know wether a node has been explored at this point
/// it is like the current vision but not reset when moving or dying
void updateExplorationGridFromMovement(State &state_p, Entity const *ent_p, long dx, long dy);

/// @brief get the vision grid of the given team
VisionGrid * getVisionGrid(State &state_p, unsigned long team_p);

/// @brief Check if all case of the entity are explored for the given player (only if static entity)
bool checkExplored(State const &state_p, Entity const *ent_p, unsigned long player_p);

/// @brief check if the grid elements corresponding to the entity
/// are free
bool checkGrid(State const &state_p, Entity const *ent_p, bool ignoreAbandonedTemples_p);

/// @brief check if the grid is free on the given coordinate
/// if forTemple_p is true a grid nod is free iif an abandonned temple is present on node
bool checkGridNode(State const &state_p, int x_p, int y_p, bool forTemple_p);

/// @brief Check that the entity does no go out of bound
/// @note always true if entity is not static
bool noOutOfBounds(State const &state_p, Entity const &ent_p);

/// @brief get all entities that are in the given box, if includeRays_p is true will compute intersection with the ray
/// of the entity
std::vector<Entity const *> getAllEntitiesInBox(Box<Fixed> const &box_p, State const &state_p, bool includeRays_p);

} // namespace octopus

#endif
