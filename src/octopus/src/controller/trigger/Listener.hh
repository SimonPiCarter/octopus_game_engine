#ifndef __Listener__
#define __Listener__

#include <vector>
#include "state/Handle.hh"

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

	virtual ListenerData * newData(Handle const triggerHandle_p, Handle const listenerHandle_p) const = 0;
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

}

#endif
