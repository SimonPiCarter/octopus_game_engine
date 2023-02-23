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

bool VisionHandler::isVisible(unsigned long team_p, unsigned long x, unsigned long y) const
{
	if(team_p < _grid.size())
	{
		return _grid.at(team_p)->isVisible(x, y);
	}
	return false;
}

void VisionHandler::updateVision(const State &state_p, const Entity &ent_p, bool set_p)
{
	Player const *player_l = state_p.getPlayer(ent_p._player);
	while(player_l->_team >= _grid.size())
	{
		_grid.push_back(new VisionGrid(_size));
	}

	_grid.at(player_l->_team)->updateVision(ent_p, set_p);
}

} // octopus
