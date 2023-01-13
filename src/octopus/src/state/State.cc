#include "State.hh"

#include "entity/Entity.hh"

namespace octopus
{

State::State() {}
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
	return _entities.at(handle_p);
}

Entity const *State::getEntity(Handle const &handle_p) const
{
	return _entities.at(handle_p);
}

bool State::hasCommandable(Handle const &handle_p) const
{
	return _commandables.size() > handle_p;
}
Commandable *State::getCommandable(Handle const &handle_p)
{
	return _commandables.at(handle_p);
}
Commandable const *State::getCommandable(Handle const &handle_p) const
{
	return _commandables.at(handle_p);
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

}
