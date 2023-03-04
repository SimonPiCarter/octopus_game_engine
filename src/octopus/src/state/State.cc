#include "State.hh"

#include "logger/Logger.hh"

#include "controller/trigger/Trigger.hh"
#include "player/Player.hh"
#include "entity/Entity.hh"
#include "entity/Resource.hh"
#include "entity/Building.hh"
#include "utils/Box.hh"
#include "state/model/entity/BuildingModel.hh"

namespace octopus
{

State::State() : _id(0), _gridSize(50), _gridPointSize(1), _pathGrid(_gridSize*_gridPointSize, _gridSize*_gridPointSize, 1., 1.), _visionHandler(_gridSize*_gridPointSize)
{
	_grid.reserve(_gridSize);
	for(size_t i = 0 ; i < _gridSize ; ++ i)
	{
		_grid.push_back(std::vector<AbstractBitset *>());
		for(size_t j = 0 ; j < _gridSize ; ++ j)
		{
			_grid.at(i).emplace_back(new SetBitset());
		}
	}
}

State::State(unsigned long id_p, unsigned long gridSize_p, unsigned long gridPointSize_p)
	: _id(id_p), _gridSize(gridSize_p), _gridPointSize(gridPointSize_p), _pathGrid(_gridSize*_gridPointSize, _gridSize*_gridPointSize, 1., 1.), _visionHandler(_gridSize*_gridPointSize)
{
	_grid.reserve(_gridSize);
	for(size_t i = 0 ; i < _gridSize ; ++ i)
	{
		_grid.push_back(std::vector<AbstractBitset *>());
		for(size_t j = 0 ; j < _gridSize ; ++ j)
		{
			_grid.at(i).emplace_back(new SetBitset());
		}
	}
}

State::~State()
{
	for(Entity * ent_l : _entities)
	{
		delete ent_l;
	}
	for(Player * player_l : _players)
	{
		delete player_l;
	}
	for(size_t i = 0 ; i < _gridSize ; ++ i)
	{
		for(size_t j = 0 ; j < _gridSize ; ++ j)
		{
			delete _grid.at(i).at(j);
		}
	}
	for(TriggerData *triggerData_l : _triggersData)
	{
		delete triggerData_l;
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

std::vector<std::vector<AbstractBitset *> > const & State::getGrid() const
{
	return _grid;
}

ListenerData * State::getListenerData(Handle const &handleTrigger_p, Handle const &handleListener_p)
{
	return getTriggerData(handleTrigger_p)->_listenerData[handleListener_p];
}
const ListenerData * State::getListenerData(Handle const &handleTrigger_p, Handle const &handleListener_p) const
{
	return getTriggerData(handleTrigger_p)->_listenerData[handleListener_p];
}

TriggerData * State::getTriggerData(Handle const &handleTrigger_p)
{
	return _triggersData[handleTrigger_p];
}
const TriggerData * State::getTriggerData(Handle const &handleTrigger_p) const
{
	return _triggersData[handleTrigger_p];
}

std::vector<std::vector<AbstractBitset *> > & State::getGrid()
{
	return _grid;
}

unsigned long State::getGridSize() const
{
	return _gridSize;
}

unsigned long State::getWorldSize() const
{
	return _gridSize * _gridPointSize;
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

long State::getGridIndex(Fixed idx_p) const
{
	long size_l = getGridSize();
	return std::min(std::max(0l, long(to_int(idx_p/_gridPointSize))), size_l-1);
}

Entity const * lookUpNewBuffTarget(State const &state_p, Handle const &sourceHandle_p, double range_p, TyppedBuff const &buff_p)
{
	Entity const * source_l = state_p.getEntity(sourceHandle_p);
	Fixed matchDistance_l = range_p + source_l->_model._ray;
	Fixed sqDis_l = 0.;
	unsigned long timeSinceBuff_l = 0;
	Entity const * best_l = nullptr;
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	Logger::getDebug() << " lookUpNewBuffTarget :: start"<< std::endl;

	Box<long> box_l {state_p.getGridIndex(source_l->_pos.x - matchDistance_l),
					 state_p.getGridIndex(source_l->_pos.x + matchDistance_l),
					 state_p.getGridIndex(source_l->_pos.y - matchDistance_l),
					 state_p.getGridIndex(source_l->_pos.y + matchDistance_l)};
	std::vector<bool> bitset_l(state_p.getEntities().size(), false);

	// grid for fast access
	std::vector<std::vector<AbstractBitset *> > const & grid_l = state_p.getGrid();

	for(long x = box_l._lowerX ; x <= box_l._upperX; ++x)
	{
		for(long y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
			// for now look for closest entity
			grid_l[x][y]->for_each([&] (int handle_p)
			{
				if(bitset_l.at(handle_p))
				{
					return false;
				}
				bitset_l[handle_p] = true;
				Entity const * ent_l = state_p.getEntity(handle_p);
				Fixed curSqDis_l = square_length(ent_l->_pos - source_l->_pos);
				if(ent_l != source_l
				&& ent_l->_alive
				&& buff_p.isApplying(state_p, *source_l, *ent_l)
				&& curSqDis_l <= range_p * range_p)
				{
					unsigned long curTimeSinceBuff_l = ent_l->getTimeSinceBuff(buff_p._id);
					// update best if no best yet
					// if time since buff is longer
					// or if time is equal but target is closer
					if(best_l == nullptr
					|| curTimeSinceBuff_l > timeSinceBuff_l
					|| (curTimeSinceBuff_l == timeSinceBuff_l && sqDis_l > curSqDis_l) )
					{
						best_l = ent_l;
						sqDis_l = curSqDis_l;
						timeSinceBuff_l = curTimeSinceBuff_l;
					}
				}
				return false;
			});
		}
	}
	return best_l;
}

Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p)
{
	double matchDistance_l = 5;

	Fixed sqDis_l = 0.;
	Entity const * closest_l = nullptr;

	Fixed sqDisBuilding_l = 0.;
	Entity const * closestBuilding_l = nullptr;

	Entity const * source_l = state_p.getEntity(sourceHandle_p);
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	Logger::getDebug() << " lookUpNewTarget :: start"<< std::endl;

	Box<long> box_l {state_p.getGridIndex(source_l->_pos.x - matchDistance_l),
					 state_p.getGridIndex(source_l->_pos.x + matchDistance_l),
					 state_p.getGridIndex(source_l->_pos.y - matchDistance_l),
					 state_p.getGridIndex(source_l->_pos.y + matchDistance_l)};
	std::vector<bool> bitset_l(state_p.getEntities().size(), false);

	// grid for fast access
	std::vector<std::vector<AbstractBitset *> > const & grid_l = state_p.getGrid();

	for(long x = box_l._lowerX ; x <= box_l._upperX; ++x)
	{
	for(long y = box_l._lowerY ; y <= box_l._upperY; ++y)
	{
	// for now look for closest entity
	grid_l[x][y]->for_each([&] (int handle_p)
	{
		if(bitset_l.at(handle_p))
		{
			return false;
		}
		bitset_l[handle_p] = true;
		Entity const * ent_l = state_p.getEntity(handle_p);
		if(ent_l == source_l
		|| !ent_l->_alive
		|| team_l == state_p.getPlayer(ent_l->_player)->_team)
		{
			// NA
		}
		else if(ent_l->_model._isBuilding)
		{
			Fixed curSqDis_l = square_length(ent_l->_pos - source_l->_pos);
			if(closestBuilding_l == nullptr
			|| sqDisBuilding_l > curSqDis_l)
			{
				closestBuilding_l = ent_l;
				sqDisBuilding_l = curSqDis_l;
			}
		}
		else if(ent_l->_model._isUnit)
		{
			Fixed curSqDis_l = square_length(ent_l->_pos - source_l->_pos);
			if(closest_l == nullptr
			|| sqDis_l > curSqDis_l)
			{
				closest_l = ent_l;
				sqDis_l = curSqDis_l;
			}
		}
		return false;
	});
	}
	}
	// reset target if too far
	if(sqDis_l > matchDistance_l*matchDistance_l + 1e-5)
	{
		Logger::getDebug() << " lookUpNewTarget :: reset because too far (unit) "<< std::endl;
		closest_l = nullptr;
	}
	// reset target if too far
	if(sqDisBuilding_l > matchDistance_l*matchDistance_l + 1e-5)
	{
		Logger::getDebug() << " lookUpNewTarget :: reset because too far (building) "<< std::endl;
		closestBuilding_l = nullptr;
	}
	if(closest_l)
	{
		Logger::getDebug() << " lookUpNewTarget :: found unit "<< std::endl;
	}
	else if(closestBuilding_l)
	{
		Logger::getDebug() << " lookUpNewTarget :: found building "<< std::endl;
	}
	else
	{
		Logger::getDebug() << " lookUpNewTarget :: found none "<< std::endl;
	}
	return closest_l?closest_l:closestBuilding_l;
}


Entity const * lookUpDeposit(State const &state_p, Handle const &sourceHandle_p, Handle const &res_p)
{
	Fixed sqDis_l = 0.;
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
		Building const * building_l = dynamic_cast<Building const *>(ent_l);
		if(!building_l->isBuilt())
		{
			continue;
		}
		BuildingModel const & model_l = building_l->_buildingModel;
		if(!model_l.isDeposit(origRes_l->_type))
		{
			continue;
		}

		Fixed curSqDis_l = square_length(ent_l->_pos - source_l->_pos);
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
	Fixed sqDis_l = 0.;
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

		Fixed curSqDis_l = square_length(ent_l->_pos - origRes_l->_pos);
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

double safeGetInMapResource(std::map<ResourceType, double> const &map_p, ResourceType type_p)
{
	auto && it_l = map_p.find(type_p);
	double res_l = 0.;
	if(it_l != map_p.end())
	{
		res_l = it_l->second;
	}
	return res_l;
}

bool checkResource(State const &state_p, unsigned long player_p, std::map<ResourceType, double> const &cost_p, std::map<ResourceType, double> const & spent_p)
{
	Player const * player_l = state_p.getPlayer(player_p);
	for(auto && pair_l : cost_p)
	{
		double res_l = safeGetInMapResource(player_l->_resources, pair_l.first);
		double spent_l = safeGetInMapResource(spent_p, pair_l.first);
		// One resource is too much for player resources
		if(pair_l.second + spent_l > res_l + 1e-5)
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
	Box<long> box_l { state_p.getGridIndex(ent_p->_pos.x-ent_p->_model._ray),
					  state_p.getGridIndex(ent_p->_pos.x+ent_p->_model._ray+0.999),
					  state_p.getGridIndex(ent_p->_pos.y-ent_p->_model._ray),
					  state_p.getGridIndex(ent_p->_pos.y+ent_p->_model._ray+0.999)
					};
	for(long x = box_l._lowerX ; x <= box_l._upperX; ++x)
	{
		for(long y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
			state_p.getGrid()[x][y]->set(ent_p->_handle, set_p);
		}
	}

	// update pathing grid only if entity is a building
	if(ent_p->_model._isStatic)
	{
		state_p.incrementPathGridStatus();
		// Canot use grid index because it uses custom size
		// because each node has a size of 1
		Box<long long> boxNode_l { to_int(std::max(Fixed(0.), ent_p->_pos.x-ent_p->_model._ray)),
								   to_int(std::max(Fixed(0.), ent_p->_pos.x+ent_p->_model._ray+0.999)),
								   to_int(std::max(Fixed(0.), ent_p->_pos.y-ent_p->_model._ray)),
								   to_int(std::max(Fixed(0.), ent_p->_pos.y+ent_p->_model._ray+0.999))
						};
		for(long long x = boxNode_l._lowerX ; x < boxNode_l._upperX; ++x)
		{
			for(long long y = boxNode_l._lowerY ; y < boxNode_l._upperY; ++y)
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

	state_p.getVisionHandler().updateVision(state_p, *ent_p, set_p);
}

void updateExplorationGrid(State &state_p, Entity const *ent_p, bool set_p)
{
	state_p.getVisionHandler().updateExploration(state_p, *ent_p, set_p);
}

bool checkGrid(State const &state_p, Entity const *ent_p, bool ignoreAbandonedTemples_p)
{
	long long size_l = state_p.getGridSize();
	// fill positional grid
	Box<long long> box_l { std::min(std::max(0ll, to_int((ent_p->_pos.x-ent_p->_model._ray))), size_l),
					  	   std::min(std::max(0ll, to_int((ent_p->_pos.x+ent_p->_model._ray+0.999))), size_l),
					  	   std::min(std::max(0ll, to_int((ent_p->_pos.y-ent_p->_model._ray))), size_l),
					  	   std::min(std::max(0ll, to_int((ent_p->_pos.y+ent_p->_model._ray+0.999))), size_l)
					};

	// only chekc grid if static
	if(ent_p->_model._isStatic)
	{
		for(long long x = box_l._lowerX ; x < box_l._upperX; ++x)
		{
			for(long long y = box_l._lowerY ; y < box_l._upperY; ++y)
			{
				GridNode const *node_l = state_p.getPathGrid().getNode(x, y);
				// only check free if we do not check abandonned temples
				if(!node_l->isFree() && !ignoreAbandonedTemples_p)
				{
					return false;
				}
				if(ignoreAbandonedTemples_p)
				{
					// if content and is not an abandonned temple
					if(node_l->getContent()
					&& !node_l->getContent()->_model._isAbandonedTemple)
					{
						return false;
					}
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
	if(!path_l.empty())
	{
		path_l.pop_front();
	}
	trimPath(path_l);
	// get waypoints
	std::list<Vector> waypoints_l = toWaypoints(path_l);
	// update for exact destination
	if(!waypoints_l.empty())
	{
		waypoints_l.pop_back();
	}
	waypoints_l.push_back(target_p);
	return waypoints_l;
}

void State::setIsOver(bool over_p)
{
	_isOver = over_p;
}

void State::setHasWinner(bool hasWinner_p)
{
	_hasWinner = hasWinner_p;
}

void State::setWinningTeam(unsigned long team_p)
{
	_winningTeam = team_p;
}

bool State::isOver() const
{
	return _isOver;
}

bool State::hasWinningTeam() const
{
	return _hasWinner;
}

unsigned long State::getWinningTeam() const
{
	return _winningTeam;
}

} // octopus
