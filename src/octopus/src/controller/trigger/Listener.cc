#include "Listener.hh"

#include "controller/event/EventCollection.hh"
#include "controller/event/EventEntityModelDied.hh"
#include "controller/event/EventEntityModelFinished.hh"

#include "state/entity/Entity.hh"
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
		if((_model == nullptr || &event_l->_model == _model) && event_l->_player == _player)
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

void ListenerEntityDied::compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const
{
	ListenerEntityData const &entData_l = static_cast<ListenerEntityData const &>(data_p);
	unsigned long count_l = 0;
	for(EventEntityModelDied const *event_l : controller_p._listEventEntityModelDied)
	{
		// handle comparison
		if(_handles.find(event_l->_entity._handle) != _handles.end())
		{
			step_p.addSteppable(new TriggerEntityAddStep(data_p._triggerHandle, data_p._listenerHandle, &event_l->_entity));
			++count_l;
		}
	}
	if(entData_l._entities.size() + count_l == _handles.size() )
	{
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
	}
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

void ListenerEntityInBox::compile(EventCollection const &controller_p, Step &step_p, bool count_p, ListenerData const &data_p) const
{
	unsigned long count_l = 0;
	State const & state_l = controller_p.getState();

	for(Handle const handle_l : _handles)
	{
		if(state_l.isEntityAlive(handle_l))
		{
			Entity const * ent_l = state_l.getLoseEntity(handle_l.index);
			Vector const &pos_l = ent_l->_pos;
			if (pos_l.x >= _boxPosition.x && pos_l.x <= _boxPosition.x + _boxSize.x
			 && pos_l.y >= _boxPosition.y && pos_l.y <= _boxPosition.y + _boxSize.y)
			{
				++count_l;
				break;
			}
		}
	}

	if(count_l > 0)
	{
		// always increment by just one!
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
	}
}

void ListenerEntityInBox::reset(Step &step_p, ListenerData const &data_p) const
{
	Listener::reset(step_p, data_p);
}

template<>
void ListenerResource<true>::compile(EventCollection const &controller_p, Step &step_p, bool, ListenerData const &data_p) const
{
	State const &state_l = controller_p.getState();

	Fixed res_l = getResource(*state_l.getPlayer(_player), _resource);
	if(res_l >= _qty)
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
}

template<>
void ListenerResource<false>::compile(EventCollection const &controller_p, Step &step_p, bool, ListenerData const &data_p) const
{
	State const &state_l = controller_p.getState();

	Fixed res_l = getResource(*state_l.getPlayer(_player), _resource);
	if(res_l <= _qty)
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
}

void ListenerUpgrade::compile(EventCollection const &controller_p, Step &step_p, bool, ListenerData const &data_p) const
{
	State const &state_l = controller_p.getState();
	Player const *player_l = state_l.getPlayer(_player);

	unsigned long lvl_p = getUpgradeLvl(*player_l, _upgrade);
	if(lvl_p >= _level)
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
}

ListenerZone *ListenerZone::newListenerZonePlayer(unsigned long player_p, Box<long> const &zone_p)
{
	return new ListenerZone(player_p, 0, true, false, zone_p);
}

ListenerZone *ListenerZone::newListenerZoneTeam(unsigned long team_p, Box<long> const &zone_p)
{
	return new ListenerZone(0, team_p, false, true, zone_p);
}

void ListenerZone::compile(EventCollection const &collection_p, Step &step_p, bool count_p, ListenerData const &data_p) const
{
	State const &state_l = collection_p.getState();
	Box<long> box_l {state_l.getGridIndex(_zone._lowerX),
					 state_l.getGridIndex(_zone._upperX),
					 state_l.getGridIndex(_zone._lowerY),
					 state_l.getGridIndex(_zone._upperY)};
	std::vector<bool> bitset_l(state_l.getEntities().size(), false);

	bool found_l = false;

	// grid for fast access
	std::vector<std::vector<AbstractBitset *> > const & grid_l = state_l.getGrid();

	for(long x = box_l._lowerX ; x <= box_l._upperX && !found_l ; ++x)
	{
		for(long y = box_l._lowerY ; y <= box_l._upperY && !found_l ; ++y)
		{
			// for now look for closest entity
			grid_l[x][y]->for_each([&] (int handle_p)
			{
				// avoid double count
				if(bitset_l.at(handle_p))
				{
					return false;
				}
				bitset_l[handle_p] = true;
				Entity const * ent_l = state_l.getLoseEntity(handle_p);
				Player const * player_l = state_l.getPlayer(ent_l->_player);

				if(ent_l->_pos.x < _zone._lowerX || ent_l->_pos.x > _zone._upperX
				|| ent_l->_pos.y < _zone._lowerY || ent_l->_pos.y > _zone._upperY)
				{
					return false;
				}

				if(player_l->_id == _player && _is_player)
				{
					found_l = true;
					return true;
				}
				if(player_l->_team == _team && _is_team)
				{
					found_l = true;
					return true;
				}
				return false;
			}
			);
		}
	}

	if(found_l)
	{
		step_p.addSteppable(new TriggerCountChange(data_p._triggerHandle, data_p._listenerHandle, data_p._count, data_p._count+1));
	}
}


} // octopus
