#ifndef __Spawner__
#define __Spawner__

#include <vector>

namespace octopus
{
class Grid;

class SpawningGrid
{
public:
	SpawningGrid(unsigned long size_p);

	/// @brief updated internal grid
	void fillGrid(unsigned long x, unsigned long y, unsigned long size_p);

	/// @brief check if grid is free
	bool isFree(unsigned long x, unsigned long y, unsigned long size_p) const;

	unsigned long getSize() const;

private:
	/// @brief internal grid
	std::vector<std::vector<bool> > _grid;

	unsigned long const _size;
};

/// @brief represent a spawn option available on
/// a spawning grid
struct Option
{
	unsigned long x;
	unsigned long y;
};

/// @brief return a vector of valid options on the spawning grid
std::vector<Option> getOptions(SpawningGrid const &grid_p, unsigned long size_p);

/// @brief return a vector of valid options on a smaller area in the spawning grid
std::vector<Option> getOptions(unsigned long x, unsigned long y, unsigned long width_p, unsigned long height_p,
	SpawningGrid const &grid_p, unsigned long size_p);

/// @brief fill the spawning grid based on the given input grid
/// based on existing blocked nodes in the world
/// @param x position of the spawning grid in the world
/// @param y position of the spawning grid in the world
/// @param grid_p grid to initialise
/// @param inputGrid_p the grid containing the world information
void initGrid(unsigned long x, unsigned long y, SpawningGrid &grid_p, Grid const &inputGrid_p);

} // namespace octopus


#endif
