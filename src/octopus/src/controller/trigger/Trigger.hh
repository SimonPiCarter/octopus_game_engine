#ifndef __Trigger__
#define __Trigger__

#include <list>
#include <vector>

#include "state/Handle.hh"

namespace octopus
{
struct CommandContext;
class EventCollection;
class Listener;
struct ListenerData;
class State;
class Step;
class Steppable;

struct TriggerData
{
	~TriggerData();
	bool _isEnabled {true};
	std::vector<ListenerData*> _listenerData;
};

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
class Trigger
{
public:
	Trigger(std::list<Listener *> const &listeners_p, bool isOneShot_p);
	virtual ~Trigger();

	/// @brief update every listener based on the controller events
	void compile(EventCollection const &collection_p, Step &step_p, TriggerData const &data_p) const;

	/// @brief check if complete if all listener are completed
	bool isComplete(TriggerData const &data_p) const;

	/// @brief reset count for all listeners
	void reset(Step &step_p, TriggerData const &data_p) const;

	/// @brief return the minimum count among all triggers
	unsigned long getCount(TriggerData const &data_p) const;


	TriggerData * newTriggerData(Handle const &triggerHandle_p) const;

	/// @brief trigger
	/// @param step_p the step to fill with steps from the trigger
	/// @param count_p the current count of the trigger in the same step (this can be used to access data for this occurence of trigger)
	virtual void trigger(State const &, CommandContext const &, Step &step_p, unsigned long count_p, TriggerData const &data_p) const = 0;

	std::list<Listener *> const _listeners;

	bool const _isOneShot;
};

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
class OneShotTrigger : public Trigger
{
public:
	OneShotTrigger(std::list<Listener *> const &listeners_p);
};

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
/// A persistent trigger create a new steppable
/// on each completion and reset all listeners on completion
class OnEachTrigger : public Trigger
{
public:
	OnEachTrigger(Listener * listener_p);
};

}

#endif
