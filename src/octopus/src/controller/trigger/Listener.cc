#include "Listener.hh"

#include "controller/event/EventCollection.hh"
#include "controller/event/EventEntityModelDied.hh"
#include "controller/event/EventEntityModelFinished.hh"

namespace octopus
{

bool Listener::isCompleted() const { return _completed; }
unsigned long Listener::getCount() const { return _count; }

void ListenerStepCount::complete(EventCollection const &controller_p, bool count_p)
{
	++_elapsedStep;
	if(_elapsedStep >= _stepCount)
	{
		_completed = true;
		++_count;
	}
}

void ListenerEntityModelDied::complete(EventCollection const &controller_p, bool count_p)
{
	_count = 0;
	for(EventEntityModelDied const *event_l : controller_p._listEventEntityModelDied)
	{
		// pointer comparison
		if(&event_l->_model == _model && event_l->_player == _player)
		{
			_completed = true;
			if(!count_p)
			{
				break;
			}
			++_count;
		}
	}
}

void ListenerEntityModelFinished::complete(EventCollection const &controller_p, bool count_p)
{
	_count = 0;
	for(EventEntityModelFinished const *event_l : controller_p._listEventEntityModelFinished)
	{
		// pointer comparison
		if(&event_l->_model == _model && event_l->_player == _player)
		{
			_completed = true;
			if(!count_p)
			{
				break;
			}
			++_count;
		}
	}
}

} // octopus
