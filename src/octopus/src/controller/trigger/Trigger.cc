#include "Trigger.hh"

#include "Listener.hh"

namespace octopus
{

Trigger::Trigger(std::list<Listener *> const &listeners_p)
	: _listeners(listeners_p)
{}

Trigger::~Trigger()
{
	for(Listener * listener_l : _listeners)
	{
		delete listener_l;
	}
}

void Trigger::complete(EventCollection const &collection_p)
{
	_isCompleted = true;
	for(Listener * listener_l : _listeners)
	{
		if(!listener_l->isCompleted())
		{
			listener_l->complete(collection_p, false);
			// updte is completed
			_isCompleted &= listener_l->isCompleted();
		}
	}
}

bool Trigger::isCompleted() const
{
	return _isCompleted;
}

void Trigger::setIsDisabled(bool isDisabled_p)
{
	_isDisabled = isDisabled_p;
}

bool Trigger::isDisabled() const
{
	return _isDisabled;
}

OneShotTrigger::OneShotTrigger(std::list<Listener *> const &listeners_p, Steppable * steppable_p)
	: Trigger(listeners_p)
	, _steppable(steppable_p)
{}

OnEachTrigger::OnEachTrigger(Listener *listeners_p)
	: _listener(listeners_p)
{}

OnEachTrigger::~OnEachTrigger()
{
	delete _listener;
}

}
