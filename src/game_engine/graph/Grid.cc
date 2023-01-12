#include "Grid.hh"

namespace game_engine
{

std::vector<std::vector<GridNode *> > setUpGrid(size_t sizeX_p, size_t sizeY_p, double stepX_p, double stepY_p)
{
	std::vector<std::vector<GridNode *> > vec_l(sizeX_p);
	for(size_t x = 0 ; x < sizeX_p ; ++ x)
	{
		for(size_t y = 0 ; y < sizeY_p ; ++ y)
		{
			vec_l.at(x).push_back(new GridNode({x * stepX_p, y * stepY_p}));
		}
	}
	return vec_l;
}

Grid::Grid(size_t sizeX_p, size_t sizeY_p, double stepX_p, double stepY_p) :
	_internalGrid(setUpGrid(sizeX_p, sizeY_p, stepX_p, stepY_p)),
	_graph(_internalGrid)
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

Graph &Grid::getGraph()
{
	return _graph;
}

Graph const &Grid::getGraph() const
{
	return _graph;
}

GridNode * Grid::getNode(size_t x, size_t y)
{
	return _internalGrid.at(x).at(y);
}

GridNode const * Grid::getNode(size_t x, size_t y) const
{
	return _internalGrid.at(x).at(y);
}


}
