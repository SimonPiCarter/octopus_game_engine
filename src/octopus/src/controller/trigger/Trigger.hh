#ifndef __Trigger__
#define __Trigger__

#include <list>

namespace octopus
{
class EventCollection;
class Listener;
class Steppable;

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
class Trigger
{
public:
	Trigger(std::list<Listener *> const &listeners_p);
	virtual ~Trigger();

	/// @brief update every listener based on the controller events
	void complete(EventCollection const &collection_p);

	bool isCompleted() const;

	/// @brief update boolean for disabled
	void setIsDisabled(bool isDisabled_p);
	/// @brief if true trigger is disabled and should not be run anymore
	bool isDisabled() const;

	std::list<Listener *> const _listeners;
private:
	bool _isCompleted {false};
	bool _isDisabled {false};
};

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
class OneShotTrigger : public Trigger
{
public:
	OneShotTrigger(std::list<Listener *> const &listeners_p, Steppable * steppable_p);
	Steppable * const _steppable;
};

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
/// A persistent trigger create a new steppable
/// on each completion and reset all listeners on completion
class OnEachTrigger
{
public:
	OnEachTrigger(Listener * listener_p);
	virtual ~OnEachTrigger();

	virtual Steppable * newSteppable() = 0;

	Listener * const _listener;
};

}

#endif
