#ifndef __GRID__
#define __GRID__

#include "GridNode.hh"
#include <vector>

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
	Grid(uint32_t sizeX_p, uint32_t sizeY_p, Fixed stepX_p, Fixed stepY_p, bool noGraph_p=false);

	~Grid();

	GridNode const * getNode(Vector const &vec_p) const;

	GridNode * getNode(uint32_t x, uint32_t y);
	GridNode const * getNode(uint32_t x, uint32_t y) const;

	uint32_t getSizeX() const { return _sizeX; }
	uint32_t getSizeY() const { return _sizeY; }

	Fixed getStepX() const { return _stepX; }
	Fixed getStepY() const { return _stepY; }

	std::vector<std::vector<GridNode *> > const & getInternalGrid() const { return _internalGrid; }
private:
	/// @brief all grid node storred by indexes
	std::vector<std::vector<GridNode *> > _internalGrid;

	uint32_t const _sizeX;
	uint32_t const _sizeY;

	Fixed const _stepX;
	Fixed const _stepY;
};

} // namespace octopus

#endif
