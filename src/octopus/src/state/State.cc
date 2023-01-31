#include "State.hh"

#include "player/Player.hh"
#include "entity/Entity.hh"
#include "entity/Resource.hh"
#include "utils/Box.hh"
#include "state/model/entity/BuildingModel.hh"

namespace octopus
{

State::State() : _id(0), _gridSize(50), _gridBitSize(3200), _pathGrid(_gridSize, _gridSize, 1., 1.)
{
	_grid.reserve(_gridSize);
	for(size_t i = 0 ; i < _gridSize ; ++ i)
	{
		_grid.emplace_back(_gridSize, DynamicBitset(_gridBitSize));
	}
}

State::State(unsigned long id_p) : _id(id_p), _gridSize(50), _gridBitSize(3200), _pathGrid(_gridSize, _gridSize, 1., 1.)
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

std::vector<Player *> &State::getPlayers()
{
	return _players;
}
std::vector<Player *> const &State::getPlayers() const
{
	return _players;
}
Player *State::getPlayer(unsigned long player_p)
{
	return _players[player_p];
}
Player const *State::getPlayer(unsigned long player_p) const
{
	return _players[player_p];
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
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	// for now look for closest entity
	for(Entity const * ent_l : state_p.getEntities())
	{
		if(ent_l == source_l
		|| !ent_l->_alive
		|| !ent_l->_model._isUnit
		|| team_l == state_p.getPlayer(ent_l->_player)->_team)
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


Entity const * lookUpDeposit(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p)
{
	double sqDis_l = 0.;
	Entity const * closest_l = nullptr;
	Entity const * source_l = state_p.getEntity(sourceHandle_p);

	Resource const * origRes_l = dynamic_cast<Resource const *>(state_p.getEntity(res_p));

	// for now look for closest entity
	for(Entity const * ent_l : state_p.getEntities())
	{
		if(ent_l == source_l
		|| !ent_l->_alive
		|| !ent_l->_model._isBuilding
		|| source_l->_player != ent_l->_player)
		{
			continue;
		}
		BuildingModel const * model_l = dynamic_cast<BuildingModel const *>(&ent_l->_model);
		if(!model_l->_deposit.at(origRes_l->_type))
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
	return closest_l;
}

Entity const * lookUpNewResource(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p)
{
	double sqDis_l = 0.;
	Entity const * closest_l = nullptr;
	Entity const * source_l = state_p.getEntity(sourceHandle_p);

	Resource const * origRes_l = dynamic_cast<Resource const *>(state_p.getEntity(res_p));

	// for now look for closest entity
	for(Entity const * ent_l : state_p.getEntities())
	{
		if(ent_l == source_l
		|| !ent_l->_alive
		|| !ent_l->_model._isResource)
		{
			continue;
		}

		Resource const * res_l = dynamic_cast<Resource const *>(ent_l);
		// Skip if not correct resource
		if(res_l->_type != origRes_l->_type
		|| res_l->_resource <= 1e-3)
		{
			continue;
		}

		double curSqDis_l = square_length(ent_l->_pos - origRes_l->_pos);
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

bool checkResource(State const &state_p, unsigned long player_p, std::map<ResourceType, double> const &cost_p)
{
	Player const * player_l = state_p.getPlayer(player_p);
	for(auto && pair_l : cost_p)
	{
		// One resource is too much for player resources
		if(pair_l.second > player_l->_resources.at(pair_l.first))
		{
			return false;
		}
	}
	return true;
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
				GridNode *node_l = state_p.getPathGrid().getNode(x, y);
				if(set_p)
				{
					node_l->setContent(ent_p);
				}
				else
				{
					node_l->setContent(nullptr);
				}
				node_l->setFree(!set_p);
			}
		}
	}
}

bool checkGrid(State const &state_p, Entity const *ent_p)
{
	long size_l = state_p.getGridSize();
	// fill positional grid
	Box<long> box_l { std::min(std::max(0l, long(ent_p->_pos.x-ent_p->_model._ray)), size_l),
					  std::min(std::max(0l, long(ent_p->_pos.x+ent_p->_model._ray+0.999)), size_l),
					  std::min(std::max(0l, long(ent_p->_pos.y-ent_p->_model._ray)), size_l),
					  std::min(std::max(0l, long(ent_p->_pos.y+ent_p->_model._ray+0.999)), size_l)
					};

	// only chekc grid if static
	if(ent_p->_model._isStatic)
	{
		for(size_t x = box_l._lowerX ; x < box_l._upperX; ++x)
		{
			for(size_t y = box_l._lowerY ; y < box_l._upperY; ++y)
			{
				GridNode const *node_l = state_p.getPathGrid().getNode(x, y);
				if(!node_l->isFree())
				{
					return false;
				}
			}
		}
	}

	return true;
}

std::list<Vector> computePath(State const & state_p, Handle const &handle_p, Vector const &target_p, std::list<Entity const *> const& ignored_p)
{
	// Find grid node source
	GridNode const * source_l = state_p.getPathGrid().getNode(state_p.getEntity(handle_p)->_pos);
	// Find grid node target
	GridNode const * target_l = state_p.getPathGrid().getNode(target_p);

	// compute path
	std::list<GridNode const *> path_l = state_p.getPathGrid().getGraph().getPath(source_l, target_l, ignored_p);
	trimPath(path_l);
	// get waypoints
	std::list<Vector> waypoints_l = toWaypoints(path_l);
	// update for exact destination
	waypoints_l.pop_back();
	waypoints_l.push_back(target_p);
	return waypoints_l;
}

}
