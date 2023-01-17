#include "State.hh"

#include "entity/Entity.hh"
#include "utils/Box.hh"

namespace octopus
{

State::State() : _id(0), _gridSize(500), _gridBitSize(3200), _pathGrid(_gridSize, _gridSize, 1., 1.)
{
	_grid.reserve(_gridSize);
	for(size_t i = 0 ; i < _gridSize ; ++ i)
	{
		_grid.emplace_back(_gridSize, DynamicBitset(_gridBitSize));
	}
}

State::State(unsigned long id_p) : _id(id_p), _gridSize(500), _gridBitSize(3200), _pathGrid(_gridSize, _gridSize, 1., 1.)
{
	_grid.reserve(_gridSize);
	for(size_t i = 0 ; i < _gridSize ; ++ i)
	{
		_grid.emplace_back(_gridSize, DynamicBitset(_gridBitSize));
	}
}

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

std::vector<std::vector<DynamicBitset> > const & State::getGrid() const
{
	return _grid;
}

std::vector<std::vector<DynamicBitset> > & State::getGrid()
{
	return _grid;
}

unsigned long State::getGridSize() const
{
	return _gridSize;
}

unsigned long State::getGridBitSize() const
{
	return _gridBitSize;
}

Grid &State::getPathGrid()
{
	return _pathGrid;
}
Grid const &State::getPathGrid() const
{
	return _pathGrid;
}

void State::incrementPathGridStatus()
{
	++_pathGridStatus;
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

void updateGrid(State &state_p, Entity const *ent_p, bool set_p)
{
	long size_l = state_p.getGridSize();
	// fill positional grid
	Box<long> box_l { std::min(std::max(0l, long(ent_p->_pos.x-ent_p->_model._ray)), size_l),
					  std::min(std::max(0l, long(ent_p->_pos.x+ent_p->_model._ray+0.999)), size_l),
					  std::min(std::max(0l, long(ent_p->_pos.y-ent_p->_model._ray)), size_l),
					  std::min(std::max(0l, long(ent_p->_pos.y+ent_p->_model._ray+0.999)), size_l)
					};
	for(size_t x = box_l._lowerX ; x < box_l._upperX; ++x)
	{
		for(size_t y = box_l._lowerY ; y < box_l._upperY; ++y)
		{
			state_p.getGrid()[x][y].set(ent_p->_handle, set_p);
		}
	}

	// update pathing grid only if entity is a building
	if(ent_p->_model._isStatic)
	{
		state_p.incrementPathGridStatus();
		for(size_t x = box_l._lowerX ; x < box_l._upperX; ++x)
		{
			for(size_t y = box_l._lowerY ; y < box_l._upperY; ++y)
			{
				state_p.getPathGrid().getNode(x, y)->setFree(!set_p);
			}
		}
	}
}

}
