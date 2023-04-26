#ifndef __GRID__
#define __GRID__

#include "GridNode.hh"
#include "Graph.hh"

namespace octopus
{

/// @brief This class aims at representing the world on a grid
/// every "case/node" is free or not
/// A Node is represented by its position in the world
/// The grid allow for fast path computation, a cache may be implemented at some point
/// Case availability represent hard constraint over the world : terrain, buildings, walls
/// Grid can be updated when new obstacles are discovered
/// Usually terrain is known from the start and will never change
/// buildings are discovered or destroyed and therefore can affect the grid, shortest path must be
/// recomputed in that case
class Grid
{
public:
	/// @brief create the grid with the given dimensions
	Grid(size_t sizeX_p, size_t sizeY_p, double stepX_p, double stepY_p, bool noGraph_p=false);

	~Grid();

	Graph &getGraph();
	Graph const &getGraph() const;

	GridNode const * getNode(Vector const &vec_p) const;

	GridNode * getNode(size_t x, size_t y);
	GridNode const * getNode(size_t x, size_t y) const;

	size_t getSizeX() const { return _sizeX; }
	size_t getSizeY() const { return _sizeY; }

	double getStepX() const { return _stepX; }
	double getStepY() const { return _stepY; }

	std::vector<std::vector<GridNode *> > const & getInternalGrid() const { return _internalGrid; }
private:
	/// @brief all grid node storred by indexes
	std::vector<std::vector<GridNode *> > _internalGrid;

	/// @brief the underlying graph used for shortest paths
	Graph _graph;

	size_t const _sizeX;
	size_t const _sizeY;

	double const _stepX;
	double const _stepY;
};

} // namespace octopus

#endif
