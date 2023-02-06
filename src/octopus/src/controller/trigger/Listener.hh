#ifndef __Listener__
#define __Listener__


namespace octopus
{
class EventCollection;
class EntityModel;

/// @brief a trigger add steppable to the controller
/// when all listeners are valid
/// steppable ownership is given away on completion
class Listener
{
public:
	virtual ~Listener() {}

	/// @brief check if listener is completed based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void complete(EventCollection const &controller_p, bool count_p) = 0;

	bool isCompleted() const;
	unsigned long getCount() const;
protected:
	bool _completed {false};
	unsigned long _count {0};
};

class ListenerStepCount : public Listener
{
public:
	ListenerStepCount(unsigned long stepCount_p)
		: _stepCount(stepCount_p) {}

	/// @brief check if listener is completed based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void complete(EventCollection const &controller_p, bool count_p);

protected:
	unsigned long const _stepCount;
	unsigned long _elapsedStep {0};
};

class ListenerEntityModelDied : public Listener
{
public:
	ListenerEntityModelDied(EntityModel const * model_p, unsigned long player_p) :
		_model(model_p), _player(player_p) {}

	/// @brief check if listener is completed based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void complete(EventCollection const &controller_p, bool count_p);

	EntityModel const * const _model;
	unsigned long const _player;
};

class ListenerEntityModelFinished : public Listener
{
public:
	ListenerEntityModelFinished(EntityModel const * model_p, unsigned long player_p) :
		_model(model_p), _player(player_p) {}

	/// @brief check if listener is completed based on events in controller
	/// @param count_p if set to true will count the number of time completed
	virtual void complete(EventCollection const &controller_p, bool count_p);

	EntityModel const * const _model;
	unsigned long const _player;
};

}

#endif
