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

Entity *State::getEntity(Handle const &handle_p)
{
	return _entities.at(handle_p);
}

Entity const *State::getEntity(Handle const &handle_p) const
{
	return _entities.at(handle_p);
}
/// @brief warning handle will be modified!
void State::addEntity(Entity * ent_p)
{
	ent_p->_handle = _entities.size();
	_entities.push_back(ent_p);
}

std::vector<Entity *> const &State::getEntities() const
{
	return _entities;
}

}
