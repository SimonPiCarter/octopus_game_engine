#include "VisionHandler.hh"

#include "VisionGrid.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "utils/Box.hh"


namespace octopus
{

VisionHandler::VisionHandler(unsigned long size_p) : _size(size_p)
{}

VisionHandler::~VisionHandler()
{
	for(VisionGrid * grid_l : _grid)
	{
		delete grid_l;
	}
}

bool VisionHandler::isVisible(unsigned long team_p, const Entity &ent_p) const
{
	if(team_p < _grid.size())
	{
		return _grid.at(team_p)->isVisible(ent_p);
	}
	return false;
}


bool VisionHandler::isVisible(unsigned long team_p, unsigned long x, unsigned long y) const
{
	if(team_p < _grid.size())
	{
		return _grid.at(team_p)->isVisible(x, y);
	}
	return false;
}

bool VisionHandler::isExplored(unsigned long team_p, unsigned long x, unsigned long y) const
{
	if(team_p < _grid.size())
	{
		return _grid.at(team_p)->isExplored(x, y);
	}
	return false;
}

void VisionHandler::updateVision(const State &state_p, const Entity &ent_p, bool set_p)
{
	getGridFromPlayer(state_p, ent_p._player)->updateVision(ent_p, set_p, _patternHandler);
}

void VisionHandler::updateExploration(const State &state_p, const Entity &ent_p, bool set_p)
{
	getGridFromPlayer(state_p, ent_p._player)->updateExploration(ent_p, set_p, _patternHandler);
}

void VisionHandler::updateVisionFromMovement(const State &state_p, const Entity &ent_p, long dx, long dy)
{
	getGridFromPlayer(state_p, ent_p._player)->updateVisionFromMovement(ent_p, dx, dy, _patternHandler);
}

void VisionHandler::updateExplorationFromMovement(const State &state_p, const Entity &ent_p, long dx, long dy)
{
	getGridFromPlayer(state_p, ent_p._player)->updateExplorationFromMovement(ent_p, dx, dy, _patternHandler);
}

VisionGrid * VisionHandler::getGridFromPlayer(const State &state_p, unsigned long player_p)
{
	Player const *player_l = state_p.getPlayer(player_p);
	return getGridFromTeam(state_p, player_l->_team);
}

VisionGrid * VisionHandler::getGridFromTeam(const State &state_p, unsigned long team_p)
{
	while(team_p >= _grid.size())
	{
		_grid.push_back(new VisionGrid(_size));
	}
	return _grid.at(team_p);
}

PatternHandler &VisionHandler::getPatternHandler()
{
	return _patternHandler;
}

} // octopus
