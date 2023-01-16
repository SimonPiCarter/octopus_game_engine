#include "State.hh"

#include "entity/Entity.hh"

namespace octopus
{

State::State() : _id(0) {}
State::State(unsigned long id_p) : _id(id_p) {}
State::~State()
{
	for(Entity * ent_l : _entities)
	{
		delete ent_l;
	}
}
bool State::hasEntity(Handle const &handle_p) const
{
	return _entities.size() > handle_p;
}

Entity *State::getEntity(Handle const &handle_p)
{
	return _entities[handle_p];
}

Entity const *State::getEntity(Handle const &handle_p) const
{
	return _entities[handle_p];
}

bool State::hasCommandable(Handle const &handle_p) const
{
	return _commandables.size() > handle_p;
}
Commandable *State::getCommandable(Handle const &handle_p)
{
	return _commandables[handle_p];
}
Commandable const *State::getCommandable(Handle const &handle_p) const
{
	return _commandables[handle_p];
}

/// @brief warning handle will be modified!
Handle State::addEntity(Entity * ent_p)
{
	ent_p->_handle = _entities.size();
	_entities.push_back(ent_p);
	ent_p->_commandableHandle = _commandables.size();
	_commandables.push_back(ent_p);
	return ent_p->_handle;
}

std::vector<Entity *> const &State::getEntities() const
{
	return _entities;
}

std::vector<Commandable *> const &State::getCommandables() const
{
	return _commandables;
}

Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p)
{
	double sqDis_l = 0.;
	Entity const * closest_l = nullptr;
	Entity const * source_l = state_p.getEntity(sourceHandle_p);

	// for now look for closest entity
	for(Entity const * ent_l : state_p.getEntities())
	{
		if(ent_l == source_l)
		{
			continue;
		}
		double curSqDis_l = square_length(ent_l->_pos - source_l->_pos);
		if(closest_l == nullptr
		|| sqDis_l > curSqDis_l)
		{
			closest_l = ent_l;
			sqDis_l = curSqDis_l;
		}
	}
	// reset target if too far
	if(sqDis_l > 25)
	{
		closest_l = nullptr;
	}
	return closest_l;
}

}
