#include "VisionGrid.hh"

#include <algorithm>

#include "PatternHandler.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "utils/Box.hh"
#include "logger/Logger.hh"

namespace octopus
{

VisionGrid::VisionGrid(unsigned long size_p)
	: _size(size_p), _grid(size_p, std::vector<long long>(size_p, 0)), _exploration(size_p, std::vector<long long>(size_p, 0))
{}

/// @brief check if the given entity is visible
bool VisionGrid::isVisible(const Entity &ent_p) const
{
	Box<long long> boxNode_l { to_int(std::max(Fixed(0.), ent_p._pos.x - ent_p._model._ray)),
								to_int(std::max(Fixed(0.), ent_p._pos.x + ent_p._model._ray+0.999)),
								to_int(std::max(Fixed(0.), ent_p._pos.y - ent_p._model._ray)),
								to_int(std::max(Fixed(0.), ent_p._pos.y + ent_p._model._ray+0.999))
					};
	for(long long x = boxNode_l._lowerX ; x < boxNode_l._upperX && x < _grid.size(); ++x)
	{
		for(long long y = boxNode_l._lowerY ; y < boxNode_l._upperY && y <  _grid[x].size(); ++y)
		{
			if(isVisible(x, y))
			{
				return true;
			}
		}
	}
	return false;
}

/// @brief check if the given entity is visible
bool VisionGrid::isExplored(const Entity &ent_p) const
{
	Box<long long> boxNode_l { to_int(std::max(Fixed(0.), ent_p._pos.x - ent_p._model._ray)),
								to_int(std::max(Fixed(0.), ent_p._pos.x + ent_p._model._ray+0.999)),
								to_int(std::max(Fixed(0.), ent_p._pos.y - ent_p._model._ray)),
								to_int(std::max(Fixed(0.), ent_p._pos.y + ent_p._model._ray+0.999))
					};
	for(long long x = boxNode_l._lowerX ; x < boxNode_l._upperX && x < _grid.size(); ++x)
	{
		for(long long y = boxNode_l._lowerY ; y < boxNode_l._upperY && y <  _grid[x].size(); ++y)
		{
			if(isExplored(x, y))
			{
				return true;
			}
		}
	}
	return false;
}

bool VisionGrid::isVisible(unsigned long x, unsigned long y) const
{
	return _grid.at(x).at(y) > 0;
}

bool VisionGrid::isExplored(unsigned long x, unsigned long y) const
{
	return _exploration.at(x).at(y) > 0;
}

void updateGrid(const Entity &ent_p, bool set_p, std::vector<std::vector<long long> > &grid_p, VisionPattern const &pattern_p)
{
	for(std::pair<long, long> const &pair_l : pattern_p)
	{
		unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first+ent_p._pos.x), grid_p.size()-1));
		unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second+ent_p._pos.y), grid_p[x].size()-1));

		if(set_p)
		{
			++grid_p[x][y];
		}
		else
		{
			--grid_p[x][y];
		}
	}
}

void VisionGrid::updateVision(const Entity &ent_p, bool set_p, PatternHandler &patternHandler_p)
{
	VisionPattern const &pattern_l = patternHandler_p.getPattern(ent_p._model._lineOfSight);
	updateGrid(ent_p, set_p, _grid, pattern_l);
}

void VisionGrid::updateExploration(const Entity &ent_p, bool set_p, PatternHandler &patternHandler_p)
{
	VisionPattern const &pattern_l = patternHandler_p.getPattern(ent_p._model._lineOfSight);
	updateGrid(ent_p, set_p, _exploration, pattern_l);
}

void VisionGrid::updateVisionFromMovement(const Entity &ent_p, long dx, long dy, PatternHandler &patternHandler_p)
{
	VisionPattern const &pattern_l = patternHandler_p.getMovementPattern(ent_p._model._lineOfSight, dx, dy);
	updateGrid(ent_p, true, _grid, pattern_l);
}

void VisionGrid::updateExplorationFromMovement(const Entity &ent_p, long dx, long dy, PatternHandler &patternHandler_p)
{
	VisionPattern const &pattern_l = patternHandler_p.getPattern(ent_p._model._lineOfSight);
	updateGrid(ent_p, true, _exploration, pattern_l);
}

void VisionGrid::incrementVisionGrid(size_t x, size_t y, long long delta_p, bool exploration_p)
{
	if(!exploration_p)
	{
		_grid[x][y] += delta_p;
	}
	else
	{
		_exploration[x][y] += delta_p;
	}
}

} // octopus
