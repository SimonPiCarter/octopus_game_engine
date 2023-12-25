#ifndef __Listener__
#define __Listener__

#include <unordered_set>
#include <vector>
#include "state/Handle.hh"
#include "utils/Box.hh"
#include "utils/Vector.hh"

namespace octopus
{
class EventCollection;
struct EntityModel;
class Entity;
class Step;

struct ListenerData
{
	ListenerData(Handle const triggerHandle_p, Handle const listenerHandle_p) : _triggerHandle(triggerHandle_p), _listenerHandle(listenerHandle_p) {}
	virtual ~ListenerData() {}

	bool isCompleted() const;
	unsigned long getCount() const;

	unsigned long _count {0};

	Handle const _triggerHandle;
	Handle const _listenerHandle;
};

/// @brief data for ListenerStepCount
/// only add a count for steps elapsed
struct ListenerStepCountData : public ListenerData
{
	ListenerStepCountData(Handle const triggerHandle_p, Handle const listenerHandle_p) : ListenerData(triggerHandle_p, listenerHandle_p) {}
	unsigned long _elapsedStep {0};
};

/// @brief data for Listeners triggering on Entity
/// this data stores all entity triggering events
struct ListenerEntityData : public ListenerData
{
	ListenerEntityData(Handle const triggerHandle_p, Handle const listenerHandle_p) : ListenerData(triggerHandle_p, listenerHandle_p) {}

	std::vector<Entity const *> _entities;
};

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
/// steppable ownership is given away on completion
class Listener
{
public:
	virtual ~Listener() {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const = 0;

	/// @brief reset the data to initial state
	virtual void reset(Step &step_p, ListenerData const &data_p) const;

	virtual ListenerData * newData(Handle const triggerHandle_p, Handle const listenerHandle_p) const
		{ return new ListenerData(triggerHandle_p, listenerHandle_p); }
};

class ListenerStepCount : public Listener
{
public:
	ListenerStepCount(unsigned long stepCount_p)
		: _stepCount(stepCount_p) {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	/// @brief reset the data to initial state
	virtual void reset(Step &step_p, ListenerData const &data_p) const override;

	virtual ListenerData * newData(Handle const triggerHandle_p, Handle const listenerHandle_p) const override
		{ return new ListenerStepCountData(triggerHandle_p, listenerHandle_p); }

protected:
	unsigned long const _stepCount;
};

class ListenerEntity : public Listener
{
public:
	/// @brief reset the data to initial state
	virtual void reset(Step &step_p, ListenerData const &data_p) const override;

	virtual ListenerData * newData(Handle const triggerHandle_p, Handle const listenerHandle_p) const override
		{ return new ListenerEntityData(triggerHandle_p, listenerHandle_p); }
};

class ListenerEntityModelDied : public ListenerEntity
{
public:
	ListenerEntityModelDied(EntityModel const * model_p, unsigned long player_p) :
		_model(model_p), _player(player_p) {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	EntityModel const * const _model;
	unsigned long const _player;
};

/// @brief listener that triggers when all entities from the given group have died
class ListenerEntityDied : public ListenerEntity
{
public:
	ListenerEntityDied(std::unordered_set<Handle> const &handles_p) :
		_handles(handles_p) {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	/// @brief the handles that triggers when they are all dead
	std::unordered_set<Handle> const _handles;
};

/// @brief
/// @note cannot extend ListenerEntity because spawn events do not have an entity created yet
class ListenerEntityModelFinished : public ListenerEntity
{
public:
	ListenerEntityModelFinished(EntityModel const * model_p, unsigned long player_p) :
		_model(model_p), _player(player_p) {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	EntityModel const * const _model;
	unsigned long const _player;
};

class ListenerEntityInBox : public Listener
{
public:
	ListenerEntityInBox(std::unordered_set<Handle> const &handles_p, Vector boxPosition_p, Vector boxSize_p)
		: _handles(handles_p), _boxPosition(boxPosition_p), _boxSize(boxSize_p) {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	/// @brief reset the data to initial state
	virtual void reset(Step &step_p, ListenerData const &data_p) const override;

	/// @brief the handles that triggers when they are all dead
	std::unordered_set<Handle> const _handles;

	/// @brief the box position
	Vector const _boxPosition;
	/// @brief the box size
	Vector const _boxSize;
};

/// @tparam Minimum if true the listener will trigger when the resource is > than the qty
template<bool Minimum>
class ListenerResource : public Listener
{
public:
	ListenerResource(unsigned long player_p, std::string const &resource_p, Fixed const &qty_p)
		: _player(player_p), _resource(resource_p), _qty(qty_p) {}

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	/// @brief player to check
	unsigned long const _player;
	/// @brief the resource to check
	std::string const _resource;
	/// @brief the quantity
	Fixed const _qty;
};

/// @brief Warning this listener can only trigger once per step and will trigger every step
/// where an entity is the zone
class ListenerZone : public Listener
{
public:
	static ListenerZone *newListenerZonePlayer(unsigned long player_p, Box<long> const &zone_p);
	static ListenerZone *newListenerZoneTeam(unsigned long team_p, Box<long> const &zone_p);

	/// @brief compile listener steps based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const override;

	unsigned long const _player;
	unsigned long const _team;
	bool const _is_player;
	bool const _is_team;
	Box<long> const _zone;
protected:
	ListenerZone(unsigned long player_p, unsigned long team_p, bool is_player_p, bool is_team_p, Box<long> const &zone_p)
		: _player(player_p), _team(team_p), _is_player(is_player_p), _is_team(is_team_p), _zone(zone_p) {}
};

} // octopus

#endif
