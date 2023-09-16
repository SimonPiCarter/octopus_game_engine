#include "State.hh"

#include <limits>
#include "logger/Logger.hh"

#include "controller/trigger/Trigger.hh"
#include "player/Player.hh"
#include "entity/Entity.hh"
#include "entity/Resource.hh"
#include "entity/Building.hh"
#include "utils/Box.hh"
#include "state/model/entity/BuildingModel.hh"
#include "command/Command.hh"

namespace octopus
{


FlyingCommandBundle::FlyingCommandBundle(CommandData * data_p, FlyingCommand * cmd_p)
	: _data(data_p)
	, _cmd(cmd_p)
{}

Handle const FlyingCommandBundle::getHandle() const { return _cmd->getHandle(); }

State::State() : _id(0), _gridSize(50), _gridPointSize(1), _pathGrid(_gridSize*_gridPointSize, _gridSize*_gridPointSize, Fixed::One(), Fixed::One()), _visionHandler(_gridSize*_gridPointSize)
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
	: _id(id_p), _gridSize(gridSize_p), _gridPointSize(gridPointSize_p), _pathGrid(_gridSize*_gridPointSize, _gridSize*_gridPointSize, Fixed::One(), Fixed::One()), _visionHandler(_gridSize*_gridPointSize)
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
	for(auto &&pair_l : _flyingCommands)
	{
		delete pair_l.second._data;
	}
}

bool State::isEntityAlive(Handle const &handle_p) const
{
	if(!hasEntity(handle_p))
	{
		return false;
	}
	Entity const * ent_l = _entities[handle_p.index];
	return ent_l->_alive;
}

bool State::hasEntity(Handle const &handle_p) const
{
	if(_entities.size() <= handle_p.index)
	{
		return false;
	}
	Entity const * ent_l = _entities[handle_p.index];
	return ent_l->_handle.revision == handle_p.revision;
}

Entity *State::getEntity(Handle const &handle_p)
{
	if(_entities[handle_p.index]->_handle.revision != handle_p.revision)
	{
		throw std::logic_error("tried to get an entity with the wrong revision number");
	}
	return _entities[handle_p.index];
}

Entity const *State::getEntity(Handle const &handle_p) const
{
	if(_entities[handle_p.index]->_handle.revision != handle_p.revision)
	{
		throw std::logic_error("tried to get an entity with the wrong revision number");
	}
	return _entities[handle_p.index];
}

Entity const *State::getLoseEntity(unsigned long handle_p) const
{
	return _entities[handle_p];
}

bool State::hasCommandable(Handle const &handle_p) const
{
	if(_commandables.size() <= handle_p.index)
	{
		return false;
	}
	Commandable const * cmdable_l = _commandables[handle_p.index];
	return cmdable_l->_commandableHandle.revision == handle_p.revision;
}
Commandable *State::getCommandable(Handle const &handle_p)
{
	if(_commandables[handle_p.index]->_commandableHandle.revision != handle_p.revision)
	{
		throw std::logic_error("tried to get a commandable with the wrong revision number");
	}
	return _commandables[handle_p.index];
}
Commandable const *State::getCommandable(Handle const &handle_p) const
{
	if(_commandables[handle_p.index]->_commandableHandle.revision != handle_p.revision)
	{
		throw std::logic_error("tried to get a commandable with the wrong revision number");
	}
	return _commandables[handle_p.index];
}

/// @brief warning handle will be modified!
Handle const &State::addEntity(Entity * ent_p)
{
	if(!_freeHandles.empty())
	{
		Handle reused_l = _freeHandles.front();
		_freeHandles.pop_front();
		ent_p->_handle = reused_l;
		ent_p->_commandableHandle = reused_l;
		// delete old entity
		delete _entities[reused_l.index];
		_entities[reused_l.index] = ent_p;
		_commandables[reused_l.index] = ent_p;
	}
	else
	{
		ent_p->_handle = Handle(_entities.size());
		_entities.push_back(ent_p);
		ent_p->_commandableHandle = Handle(_commandables.size());
		_commandables.push_back(ent_p);
	}
	return ent_p->_handle;
}

void State::addFreeHandle(Handle const &handle_p)
{
	if(handle_p.revision < std::numeric_limits<unsigned char>::max())
	{
		_queuedfreeHandles.back().push_back(handle_p);
		++_queuedfreeHandles.back().back().revision;
	}
}

void State::popBackFreeHandle()
{
	_queuedfreeHandles.back().pop_back();
}

std::list<Handle> const &State::getFreeHandles() const
{
	return _freeHandles;
}


/// @brief initialize the queued free handles with the desired size
void State::initializeQueueFreeHandles(size_t size_p)
{
	_queuedfreeHandles = std::list<std::list<Handle> >(size_p, std::list<Handle>());
}

/// @brief unfold one stack of the queued free handles
/// @note this should be called every time a step is applied (in the ticking step)
void State::unfoldQueuedFreeHandles()
{
	_freeHandles.splice(_freeHandles.end(), _queuedfreeHandles.front());
	_queuedfreeHandles.pop_front();
	_queuedfreeHandles.push_back({});
}

/// @brief restore a list of handled back at the front of the queue and clear them from the
/// free handles list (they should all be at the back of the list)
void State::refoldQueuedFreeHandles(std::list<Handle> const &handles_p)
{
	if(!handles_p.empty())
	{
		_freeHandles.erase(std::prev(_freeHandles.end(), handles_p.size()));
	}
	_queuedfreeHandles.push_front(handles_p);
	_queuedfreeHandles.pop_back();
}
/// @brief return the front queued handles (used to store some data for eventual restore)
std::list<Handle> const &State::getFrontQueuedHandles() const
{
	return _queuedfreeHandles.front();
}

std::vector<Entity *> &State::getEntities()
{
	return _entities;
}

std::vector<Entity *> const &State::getEntities() const
{
	return _entities;
}

std::vector<Commandable *> const &State::getCommandables() const
{
	return _commandables;
}

FlyingCommandSet & State::getFlyingCommands()
{
	return _flyingCommands;
}

const FlyingCommandSet & State::getFlyingCommands() const
{
	return _flyingCommands;
}

Handle State::getFlyingCommandHandle(unsigned long spawning_p) const
{
	// number of available handle found
	unsigned long offset_l = 0;
	Handle handle_l;
	while(offset_l<=spawning_p)
	{
		auto &&it_l = _flyingCommands.find(handle_l);
		if(it_l == _flyingCommands.end())
		{
			++offset_l;
		}
		++handle_l.index;
	}
	--handle_l.index;
	return handle_l;
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
	return getTriggerData(handleTrigger_p)->_listenerData[handleListener_p.index];
}
const ListenerData * State::getListenerData(Handle const &handleTrigger_p, Handle const &handleListener_p) const
{
	return getTriggerData(handleTrigger_p)->_listenerData[handleListener_p.index];
}

TriggerData * State::getTriggerData(Handle const &handleTrigger_p)
{
	return _triggersData[handleTrigger_p.index];
}
const TriggerData * State::getTriggerData(Handle const &handleTrigger_p) const
{
	return _triggersData[handleTrigger_p.index];
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
	return std::min(std::max(0l, long(to_int(idx_p/long(_gridPointSize)))), size_l-1);
}

Entity const * lookUpNewBuffTarget(State const &state_p, Handle const &sourceHandle_p, Fixed range_p, TimedBuff const &buff_p)
{
	Entity const * source_l = state_p.getEntity(sourceHandle_p);
	Fixed matchDistance_p = range_p + source_l->_model._ray;
	Fixed sqDis_l = 0.;
	unsigned long timeSinceBuff_l = 0;
	Entity const * best_l = nullptr;
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	Logger::getDebug() << " lookUpNewBuffTarget :: start"<< std::endl;

	Box<long> box_l {state_p.getGridIndex(source_l->_pos.x - matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.x + matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.y - matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.y + matchDistance_p)};
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
				Entity const * ent_l = state_p.getLoseEntity(handle_p);
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

TargetPanel lookUpNewTargets(State const &state_p, Handle const &sourceHandle_p, Fixed matchDistance_p, bool filterTeam_p)
{
	TargetPanel panel_l;
	panel_l.matchDistance = matchDistance_p;

	Entity const * source_l = state_p.getEntity(sourceHandle_p);
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	Logger::getDebug() << " lookUpNewTargets :: start"<< std::endl;

	Box<long> box_l {state_p.getGridIndex(source_l->_pos.x - matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.x + matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.y - matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.y + matchDistance_p)};
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
		Entity const * ent_l = state_p.getLoseEntity(handle_p);
		if(ent_l == source_l
		|| !ent_l->_alive
		|| ent_l->_model._invulnerable
		|| (team_l == state_p.getPlayer(ent_l->_player)->_team && filterTeam_p))
		{
			// NA
		}
		else if(ent_l->_model._isBuilding)
		{
			panel_l.buildings.push_back(ent_l);
		}
		else if(ent_l->_model._isUnit)
		{
			panel_l.units.push_back(ent_l);
		}
		return false;
	});
	}
	}

	auto remover_l = [&matchDistance_p, &source_l] (Entity const * entity_p)
	{
		Fixed curSqDis_l = square_length(entity_p->_pos - source_l->_pos);
		return curSqDis_l > matchDistance_p*matchDistance_p + 1e-5;
	};
	/// remove out of range
	panel_l.buildings.erase(std::remove_if(panel_l.buildings.begin(), panel_l.buildings.end(), remover_l), panel_l.buildings.end());
	panel_l.units.erase(std::remove_if(panel_l.units.begin(), panel_l.units.end(), remover_l), panel_l.units.end());

	return panel_l;
}

Entity const * lookUpNewTarget(State const &state_p, Handle const &sourceHandle_p, Fixed matchDistance_p, bool healing_p)
{
	Fixed sqDis_l = 0.;
	Entity const * closest_l = nullptr;

	Fixed sqDisBuilding_l = 0.;
	Entity const * closestBuilding_l = nullptr;

	Entity const * source_l = state_p.getEntity(sourceHandle_p);
	unsigned long team_l = state_p.getPlayer(source_l->_player)->_team;

	Logger::getDebug() << " lookUpNewTarget :: start"<< std::endl;

	Box<long> box_l {state_p.getGridIndex(source_l->_pos.x - matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.x + matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.y - matchDistance_p),
					 state_p.getGridIndex(source_l->_pos.y + matchDistance_p)};
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
		Entity const * ent_l = state_p.getLoseEntity(handle_p);
		bool teamCheck_l = (healing_p && team_l == state_p.getPlayer(ent_l->_player)->_team)
						|| (!healing_p && team_l != state_p.getPlayer(ent_l->_player)->_team);
		if(ent_l == source_l
		|| !ent_l->_alive
		|| ent_l->_model._invulnerable
		|| !teamCheck_l
		|| (healing_p && ent_l->_hp == ent_l->getHpMax()))
		{
			// NA
		}
		else if(ent_l->_model._isBuilding && !healing_p)
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
	if(sqDis_l > matchDistance_p*matchDistance_p + 1e-5)
	{
		Logger::getDebug() << " lookUpNewTarget :: reset because too far (unit) "<< std::endl;
		closest_l = nullptr;
	}
	// reset target if too far
	if(sqDisBuilding_l > matchDistance_p*matchDistance_p + 1e-5)
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
		if(!model_l.isDeposit(origRes_l->getType()))
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
		if(res_l->getType() != origRes_l->getType()
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

bool lookUpNonStaticBehind(State const &state_p, Handle const &handleSource_p, unsigned long width_p, unsigned long height_p)
{
	if(!state_p.isEntityAlive(handleSource_p))
	{
		return false;
	}
	Entity const * source_l = state_p.getEntity(handleSource_p);

	Logger::getDebug() << " lookUpNonStaticBehind :: start"<< std::endl;

	Box<long> box_l {state_p.getGridIndex(source_l->_pos.x - width_p),
					 state_p.getGridIndex(source_l->_pos.x + width_p),
					 state_p.getGridIndex(source_l->_pos.y - height_p),
					 state_p.getGridIndex(source_l->_pos.y)};

	// grid for fast access
	std::vector<std::vector<AbstractBitset *> > const & grid_l = state_p.getGrid();

	bool found_l = false;

	for(long x = box_l._lowerX ; x <= box_l._upperX; ++x)
	{
		for(long y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
			// if any non static
			grid_l[x][y]->for_each([&] (int handle_p)
			{
				Entity const * check_l = state_p.getLoseEntity(handle_p);

				if(!check_l->_model._isStatic
				&& check_l->_pos.x > source_l->_pos.x - width_p
				&& check_l->_pos.x < source_l->_pos.x + width_p
				&& check_l->_pos.y > source_l->_pos.y - height_p
				&& check_l->_pos.y < source_l->_pos.y)
				{
					found_l = true;
					// break the loop
					return true;
				}
				// go on
				return false;
			}
			);

			if(found_l)
			{
				return true;
			}
		}
	}

	return found_l;
}

Fixed safeGetInMapResource(std::map<std::string, Fixed> const &map_p, std::string type_p)
{
	auto && it_l = map_p.find(type_p);
	Fixed res_l = 0.;
	if(it_l != map_p.end())
	{
		res_l = it_l->second;
	}
	return res_l;
}

bool checkResource(State const &state_p, unsigned long player_p, std::map<std::string, Fixed> const &cost_p, std::map<std::string, Fixed> const & spent_p)
{
	Player const * player_l = state_p.getPlayer(player_p);
	for(auto && pair_l : cost_p)
	{
		Fixed res_l = safeGetInMapResource(player_l->_resources, pair_l.first);
		Fixed spent_l = safeGetInMapResource(spent_p, pair_l.first);
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
			state_p.getGrid()[x][y]->set(ent_p->_handle.index, set_p);
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
		Grid &pathGrid_l = state_p.getPathGrid();
		long long sizeX_l = pathGrid_l.getSizeX();
		long long sizeY_l = pathGrid_l.getSizeY();

		for(long long x = boxNode_l._lowerX ; x < boxNode_l._upperX && x < sizeX_l ; ++x)
		{
			for(long long y = boxNode_l._lowerY ; y < boxNode_l._upperY && y < sizeY_l ; ++y)
			{
				GridNode *node_l = pathGrid_l.getNode(x, y);
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

void updateVisionGrid(State &state_p, Entity const *ent_p, bool set_p)
{
	state_p.getVisionHandler().updateVision(state_p, *ent_p, set_p);
}

void updateExplorationGrid(State &state_p, Entity const *ent_p, bool set_p)
{
	state_p.getVisionHandler().updateExploration(state_p, *ent_p, set_p);
}

void updateVisionGridFromMovement(State &state_p, Entity const *ent_p, long dx, long dy)
{
	state_p.getVisionHandler().updateVisionFromMovement(state_p, *ent_p, dx, dy);
}
void updateExplorationGridFromMovement(State &state_p, Entity const *ent_p, long dx, long dy)
{
	state_p.getVisionHandler().updateExplorationFromMovement(state_p, *ent_p, dx, dy);
}

VisionGrid * getVisionGrid(State &state_p, unsigned long team_p)
{
	return state_p.getVisionHandler().getGridFromTeam(state_p, team_p);
}

Box<long long> getBox(Entity const &ent_p)
{
	return 	Box<long long> { to_int((ent_p._pos.x-ent_p._model._ray)),
						   to_int((ent_p._pos.x+ent_p._model._ray+0.999)),
						   to_int((ent_p._pos.y-ent_p._model._ray)),
						   to_int((ent_p._pos.y+ent_p._model._ray+0.999))
					};
}

bool checkGrid(State const &state_p, Entity const *ent_p, bool ignoreAbandonedTemples_p)
{
	if(!noOutOfBounds(state_p, *ent_p))
	{
		return false;
	}
	Grid const &pathGrid_l = state_p.getPathGrid();
	// fill positional grid
	Box<long long> box_l = getBox(*ent_p);

	// only check grid if static
	if(ent_p->_model._isStatic)
	{
		for(long long x = box_l._lowerX ; x < box_l._upperX; ++x)
		{
			for(long long y = box_l._lowerY ; y < box_l._upperY; ++y)
			{
				GridNode const *node_l = pathGrid_l.getNode(x, y);
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

bool checkGridNode(State const &state_p, int x_p, int y_p, bool forTemple_p)
{
	Grid const &pathGrid_l = state_p.getPathGrid();
	if(x_p >= 0
	&& x_p <= pathGrid_l.getSizeX()
	&& y_p >= 0
	&& y_p <= pathGrid_l.getSizeY())
	{
		GridNode const *node_l = pathGrid_l.getNode(x_p, y_p);
		if(forTemple_p)
		{
			// if content and is not an abandonned temple
			if(node_l->getContent()
			&& !node_l->getContent()->_model._isAbandonedTemple)
			{
				return false;
			}
			if(node_l->isFree())
			{
				return false;
			}
		}
		else if(!node_l->isFree())
		{
			return false;
		}
		return true;
	}
	// out of bound
	return false;
}


bool noOutOfBounds(State const &state_p, Entity const &ent_p)
{
	Grid const &pathGrid_l = state_p.getPathGrid();

	Box<long long> box_l = getBox(ent_p);
	if(ent_p._model._isStatic)
	{
		// check that we do not go out of bound
		return box_l._lowerX >= 0
			&& box_l._upperX <= pathGrid_l.getSizeX()
			&& box_l._lowerY >= 0
			&& box_l._upperY <= pathGrid_l.getSizeY();
	}
	return true;
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

void State::addStepApplied()
{
	++_stepApplied;
}

void State::removeStepApplied()
{
	--_stepApplied;
}

unsigned long State::getStepApplied() const
{
	return _stepApplied;
}

} // octopus
