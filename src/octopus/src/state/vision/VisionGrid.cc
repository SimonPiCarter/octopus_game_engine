#include "VisionGrid.hh"

#include <algorithm>

#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "utils/Box.hh"
#include "logger/Logger.hh"

namespace octopus
{

VisionGrid::VisionGrid(unsigned long size_p) : _grid(size_p, std::vector<long long>(size_p, 0)), _exploration(size_p, std::vector<long long>(size_p, 0))
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

void VisionGrid::updateVision(const Entity &ent_p, bool set_p)
{
	VisionPattern const &pattern_l = getPattern(ent_p._model._lineOfSight);
	updateGrid(ent_p, set_p, _grid, pattern_l);
}

void VisionGrid::updateExploration(const Entity &ent_p, bool set_p)
{
	VisionPattern const &pattern_l = getPattern(ent_p._model._lineOfSight);
	updateGrid(ent_p, set_p, _exploration, pattern_l);
}

void VisionGrid::updateVisionFromMovement(const Entity &ent_p, long dx, long dy)
{
	VisionPattern const &pattern_l = getMovementPattern(ent_p._model._lineOfSight, dx, dy);
	updateGrid(ent_p, true, _grid, pattern_l);
}

void VisionGrid::updateExplorationFromMovement(const Entity &ent_p, long dx, long dy)
{
	VisionPattern const &pattern_l = getPattern(ent_p._model._lineOfSight);
	updateGrid(ent_p, true, _exploration, pattern_l);
}

VisionPattern const &VisionGrid::getPattern(long lineOfSight_p)
{
	// check cache
	auto && it_l = _patterns.find(lineOfSight_p);
	if(it_l != _patterns.end())
	{
		return it_l->second;
	}

	VisionPattern & pattern_l = _patterns[lineOfSight_p];

	// compute a box to avoid checking full map
	Box<long> box_l { - (lineOfSight_p+1),
					  lineOfSight_p+1,
					  - (lineOfSight_p+1),
					  lineOfSight_p+1
					};

	Fixed ref_l(lineOfSight_p*lineOfSight_p);
	// check distance on all nodes in the subbox
	for(long x = box_l._lowerX; x <= box_l._upperX; ++x)
	{
		for(long y = box_l._lowerY; y <= box_l._upperY; ++y)
		{
			if(square_length(Vector(x, y)) < ref_l)
			{
				pattern_l.push_back(std::make_pair(x, y));
			}
		}
	}

	return pattern_l;
}

VisionPattern const &VisionGrid::getMovementPattern(long lineOfSight_p, long dx, long dy)
{
	// check cache
	if((dx == 0 && dy == 0)
	|| !_movingPatterns[lineOfSight_p][dx][dy].empty() )
	{
		return _movingPatterns[lineOfSight_p][dx][dy];
	}

	// get pattern for given los
	VisionPattern const & pattern_l = getPattern(lineOfSight_p);

	VisionPattern &movingPattern_l = _movingPatterns[lineOfSight_p][dx][dy];

	// list pattern for faster deletion
	std::list<std::pair<long, long> > listPattern_l(pattern_l.begin(), pattern_l.end());

	for(std::pair<long, long> const &pair_l : pattern_l)
	{
		long x = pair_l.first - dx;
		long y = pair_l.second - dy;

		listPattern_l.remove(std::make_pair(x, y));
	}
	movingPattern_l.insert(movingPattern_l.end(), listPattern_l.begin(), listPattern_l.end());

	return movingPattern_l;
}

} // octopus
