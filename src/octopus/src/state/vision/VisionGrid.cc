#include "VisionGrid.hh"

#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "utils/Box.hh"

namespace octopus
{

VisionGrid::VisionGrid(unsigned long size_p) : _grid(size_p, std::vector<long>(size_p, 0))
{}

bool VisionGrid::isVisible(unsigned long x, unsigned long y) const
{
	return _grid.at(x).at(y) > 0;
}

void VisionGrid::updateVision(const Entity &ent_p, bool set_p)
{
	// comptue a box to avoid checking full map
	Box<long> box_l { long(ent_p._pos.x - (ent_p._model._lineOfSight+1) ),
					  long(ent_p._pos.x + (ent_p._model._lineOfSight+1) ),
					  long(ent_p._pos.y - (ent_p._model._lineOfSight+1) ),
					  long(ent_p._pos.y + (ent_p._model._lineOfSight+1) )
					};

	// check distance on all nodes in the subbox
	for(size_t x = box_l._lowerX ; x <= box_l._upperX; ++x)
	{
		for(size_t y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
			double dist_l = square_length(ent_p._pos - Vector(x + 0.5, y + 0.5));
			if(dist_l < ent_p._model._lineOfSight*ent_p._model._lineOfSight)
			{
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
	}
}

} // octopus
