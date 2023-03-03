#include "VisionGrid.hh"

#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "utils/Box.hh"

namespace octopus
{

VisionGrid::VisionGrid(unsigned long size_p) : _grid(size_p, std::vector<long>(size_p, 0)), _exploration(size_p, std::vector<bool>(size_p, 0))
{}

bool VisionGrid::isVisible(unsigned long x, unsigned long y) const
{
	return _grid.at(x).at(y) > 0;
}

bool VisionGrid::isExplored(unsigned long x, unsigned long y) const
{
	return _exploration.at(x).at(y);
}

void VisionGrid::updateVision(const Entity &ent_p, bool set_p)
{
	VisionPattern const &pattern_l = getPattern(ent_p._model._lineOfSight);

	for(std::pair<unsigned long, unsigned long> const &pair_l : pattern_l)
	{
		unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first+ent_p._pos.x), _grid.size()-1));
		unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second+ent_p._pos.y), _grid[x].size()-1));

		if(set_p)
		{
			++_grid[x][y];
		}
		else
		{
			--_grid[x][y];
		}
	}
}

void VisionGrid::updateExploration(const Entity &ent_p, bool set_p)
{
	VisionPattern const &pattern_l = getPattern(ent_p._model._lineOfSight);

	for(std::pair<long, long> const &pair_l : pattern_l)
	{
		unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first+ent_p._pos.x), _grid.size()-1));
		unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second+ent_p._pos.y), _grid[x].size()-1));

		_exploration[x][y] = set_p;
	}
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

} // octopus
