#include "SpawningGrid.hh"

#include "graph/Grid.hh"

namespace octopus
{

SpawningGrid::SpawningGrid(unsigned long size_p)
	: _size(size_p)
{
	for(unsigned long i = 0 ; i < size_p; ++i)
	{
		_grid.emplace_back(size_p, false);
	}
}

/// @brief updated internal grid
void SpawningGrid::fillGrid(unsigned long x, unsigned long y, unsigned long size_p)
{
	for(unsigned long i = x ; i < x + size_p && i < _grid.size() ; ++ i)
	{
		for(unsigned long j = y ; j < y + size_p && j < _grid[i].size() ; ++ j)
		{
			_grid[i][j] = true;
		}
	}
}

/// @brief check if grid is free
bool SpawningGrid::isFree(unsigned long x, unsigned long y, unsigned long size_p) const
{
	for(unsigned long i = x ; i < x + size_p && i < _grid.size() ; ++ i)
	{
		for(unsigned long j = y ; j < y + size_p && j < _grid[i].size() ; ++ j)
		{
			if(_grid[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

unsigned long SpawningGrid::getSize() const
{
	return _size;
}

std::vector<Option> getOptions(SpawningGrid const &grid_p, unsigned long size_p)
{
	std::vector<Option> options_l;
	for(unsigned long i = 0 ; i < grid_p.getSize() - size_p + 1 ; ++ i)
	{
		for(unsigned long j = 0 ; j < grid_p.getSize() - size_p + 1 ; ++ j)
		{
			if(grid_p.isFree(i, j, size_p))
			{
				options_l.push_back({i, j});
			}
		}
	}
	return options_l;
}

std::vector<Option> getOptions(unsigned long x, unsigned long y, unsigned long width_p, unsigned long height_p,
	SpawningGrid const &grid_p, unsigned long size_p)
{
	std::vector<Option> options_l;
	for(unsigned long i = x ; i < std::min(grid_p.getSize(), x + width_p) - size_p + 1 ; ++ i)
	{
		for(unsigned long j = y ; j < std::min(grid_p.getSize(), y + height_p) - size_p + 1 ; ++ j)
		{
			if(grid_p.isFree(i, j, size_p))
			{
				options_l.push_back({i, j});
			}
		}
	}
	return options_l;
}

void initGrid(unsigned long x, unsigned long y, SpawningGrid &grid_p, Grid const &inputGrid_p)
{
	for(unsigned long i = x ; i < grid_p.getSize() + x ; ++i)
	{
		for(unsigned long j = y ; j < grid_p.getSize() + x ; ++j)
		{
			// if input grid is not free we update the grid
			if(!inputGrid_p.getNode(i, j)->isFree())
			{
				grid_p.fillGrid(i-x, j-y, 1);
			}
		}
	}
}

} // namespace octopus

