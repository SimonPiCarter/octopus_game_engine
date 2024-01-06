#include "Grid.hh"

namespace octopus
{

std::vector<std::vector<GridNode *> > setUpGrid(uint32_t sizeX_p, uint32_t sizeY_p, Fixed stepX_p, Fixed stepY_p)
{
	std::vector<std::vector<GridNode *> > vec_l(sizeX_p);
	for(uint32_t x = 0 ; x < sizeX_p ; ++ x)
	{
		for(uint32_t y = 0 ; y < sizeY_p ; ++ y)
		{
			vec_l.at(x).push_back(new GridNode({x * stepX_p + stepX_p/2., y * stepY_p + stepY_p/2.}));
		}
	}
	return vec_l;
}

Grid::Grid(uint32_t sizeX_p, uint32_t sizeY_p, Fixed stepX_p, Fixed stepY_p, bool noGraph_p) :
	_internalGrid(setUpGrid(sizeX_p, sizeY_p, stepX_p, stepY_p)),
	_sizeX(sizeX_p),
	_sizeY(sizeY_p),
	_stepX(stepX_p),
	_stepY(stepY_p)
{}

Grid::~Grid()
{
	for(std::vector<GridNode *> const &vec_l : _internalGrid)
	{
		for(GridNode * node_l : vec_l)
		{
			delete node_l;
		}
	}
}

GridNode const * Grid::getNode(Vector const &vec_p) const
{
	if(vec_p.x < 0 || vec_p.y < 0)
	{
		throw std::logic_error("Cannot handle negative coordinate");
	}
	// get index from coordinate
	uint32_t x = to_uint(vec_p.x/_stepX);
	uint32_t y = to_uint(vec_p.y/_stepY);

	return getNode(x, y);
}

GridNode * Grid::getNode(uint32_t x, uint32_t y)
{
	return _internalGrid.at(x).at(y);
}

GridNode const * Grid::getNode(uint32_t x, uint32_t y) const
{
	return _internalGrid.at(x).at(y);
}


}
