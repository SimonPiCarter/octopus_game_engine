#include "Listener.hh"

#include "controller/event/EventCollection.hh"
#include "controller/event/EventEntityModelDied.hh"
#include "controller/event/EventEntityModelFinished.hh"

#include "step/Step.hh"
#include "step/trigger/TriggerCountChange.hh"
#include "step/trigger/TriggerStepCountChange.hh"
#include "step/trigger/TriggerEntityAddStep.hh"
#include "step/trigger/TriggerEntityResetStep.hh"

namespace octopus
{

bool ListenerData::isCompleted() const
{
	return this->getCount() > 0;
}
unsigned long ListenerData::getCount() const
{
	return _count;
}

void Listener::reset(Step &step_p, ListenerData const &data_p) const
{
	step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, 0));
}

void ListenerStepCount::compile(EventCollection const &, Step &step_p, bool count_p, ListenerData const &data_p) const
{
	unsigned long steps_l = static_cast<ListenerStepCountData const &>(data_p)._elapsedStep;

	step_p.addSteppable(new TriggerStepCountChange(data_p._triggerHandle, data_p._listenerHandle, steps_l, steps_l+1));
	if(steps_l + 1 >= _stepCount)
	{
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
	}
}

void ListenerStepCount::reset(Step &step_p, ListenerData const &data_p) const
{
	Listener::reset(step_p, data_p);
	unsigned long steps_l = static_cast<ListenerStepCountData const &>(data_p)._elapsedStep;
	step_p.addSteppable(new TriggerStepCountChange(data_p._triggerHandle, data_p._listenerHandle, steps_l, 0));
}

void ListenerEntity::reset(Step &step_p, ListenerData const &data_p) const
{
	Listener::reset(step_p, data_p);
	std::vector<Entity const *> const &list_l = static_cast<ListenerEntityData const &>(data_p)._entities;
	step_p.addSteppable(new TriggerEntityResetStep(data_p._triggerHandle, data_p._listenerHandle, list_l));
}

void ListenerEntityModelDied::compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const
{
	unsigned long count_l = 0;
	for(EventEntityModelDied const *event_l : controller_p._listEventEntityModelDied)
	{
		// pointer comparison
		if(&event_l->_model == _model && event_l->_player == _player)
		{
			step_p.addSteppable(new TriggerEntityAddStep(data_p._triggerHandle, data_p._listenerHandle, &event_l->_entity));
			++count_l;
			if(!count_p)
			{
				break;
			}
		}
	}
	step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+count_l));
}

void ListenerEntityModelFinished::compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const
{
	unsigned long count_l = 0;
	for(EventEntityModelFinished const *event_l : controller_p._listEventEntityModelFinished)
	{
		// pointer comparison
		if(&event_l->_model == _model && event_l->_player == _player)
		{
			step_p.addSteppable(new TriggerEntityAddStep(data_p._triggerHandle, data_p._listenerHandle, &event_l->_entity));
			++count_l;
			if(!count_p)
			{
				break;
			}
		}
	}
	step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+count_l));
}

} // octopus
