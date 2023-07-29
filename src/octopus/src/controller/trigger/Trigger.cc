#include "Trigger.hh"

#include <limits>

#include "Listener.hh"

namespace octopus
{

TriggerData::~TriggerData()
{
	for(ListenerData* data_l : _listenerData)
	{
		delete data_l;
	}
}

Trigger::Trigger(std::list<Listener *> const &listeners_p, bool isOneShot_p)
	: _listeners(listeners_p)
	, _isOneShot(isOneShot_p)
{}

Trigger::~Trigger()
{
	for(Listener * listener_l : _listeners)
	{
		delete listener_l;
	}
}

void Trigger::compile(EventCollection const &collection_p, Step &step_p, TriggerData const &data_p) const
{
	Handle listenerHandle_l;
	for(Listener const * listener_l : _listeners)
	{
		ListenerData const * listData_l = data_p._listenerData[listenerHandle_l];
		if(!listData_l->isCompleted())
		{
			listener_l->compile(collection_p, step_p, !_isOneShot, *listData_l);
		}
		++listenerHandle_l.index;
	}
}

bool Trigger::isComplete(TriggerData const &data_p) const
{
	for(ListenerData const * listData_l : data_p._listenerData)
	{
		if(!listData_l->isCompleted())
		{
			return false;
		}
	}
	return true;
}

void Trigger::reset(Step &step_p, TriggerData const &data_p) const
{
	Handle listenerHandle_l;
	for(Listener const * listener_l : _listeners)
	{
		ListenerData const * listData_l = data_p._listenerData[listenerHandle_l];
		listener_l->reset(step_p, *listData_l);
		++listenerHandle_l.index;
	}
}

unsigned long Trigger::getCount(TriggerData const &data_p) const
{
	if(_isOneShot)
	{
		return 1;
	}

	unsigned long minCount_l = std::numeric_limits<unsigned long>::max();
	for(ListenerData const *listData_l : data_p._listenerData)
	{
		minCount_l = std::min(minCount_l, listData_l->getCount());
	}
	return minCount_l;
}


TriggerData * Trigger::newTriggerData(Handle const &triggerHandle_p) const
{
	TriggerData * data_l = new TriggerData();
	for(Listener * listener_l : _listeners)
	{
		data_l->_listenerData.push_back(listener_l->newData(triggerHandle_p, Handle(data_l->_listenerData.size())));
	}
	return data_l;
}

OneShotTrigger::OneShotTrigger(std::list<Listener *> const &listeners_p)
	: Trigger(listeners_p, true)
{}

OnEachTrigger::OnEachTrigger(Listener *listener_p)
	: Trigger({listener_p}, false)
{}

}
