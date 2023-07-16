#include "ProceduralTerrain.hh"

#include "Area.hh"
#include "SpawningGrid.hh"

namespace octopus
{

/// @brief add randomized drops around the current shape
void handleYDrops(RandomGenerator &rand_p, TerrainOption &option_p, int minRandomDrops_p, int maxRandomDrops_p)
{
	int nbDrops_l = rand_p.roll(minRandomDrops_p, maxRandomDrops_p);

	int minX_l = 0;
	int maxX_l = 0;
	for(TerrainCoord const &coord_l : option_p.cases)
	{
		minX_l = std::min(minX_l, coord_l.x);
		maxX_l = std::max(maxX_l, coord_l.x);
	}

	for(int i = 0 ; i < nbDrops_l ; ++ i)
	{
		int pos_l = rand_p.roll(minX_l, maxX_l);
		bool top_l = rand_p.roll(0, 1) > 0;

		// look for extremal value with x = pos_l
		// if left we look for max, otherwise we look for min
		int extremal_l = 0;
		for(TerrainCoord const &coord_l : option_p.cases)
		{
			if(coord_l.y == pos_l)
			{
				if(top_l && coord_l.y > extremal_l)
				{
					extremal_l = coord_l.y;
				}
				if(!top_l && coord_l.y < extremal_l)
				{
					extremal_l = coord_l.y;
				}
			}
		}
		if(top_l)
		{
			++extremal_l;
		}
		else
		{
			++extremal_l;
		}

		option_p.cases.push_back({extremal_l, pos_l});
	}
}

/// @brief add randomized drops around the current shape
void handleXDrops(RandomGenerator &rand_p, TerrainOption &option_p, int minRandomDrops_p, int maxRandomDrops_p)
{
	int nbDrops_l = rand_p.roll(minRandomDrops_p, maxRandomDrops_p);

	int minY_l = 0;
	int maxY_l = 0;
	for(TerrainCoord const &coord_l : option_p.cases)
	{
		minY_l = std::min(minY_l, coord_l.y);
		maxY_l = std::max(maxY_l, coord_l.y);
	}

	for(int i = 0 ; i < nbDrops_l ; ++ i)
	{
		int pos_l = rand_p.roll(minY_l, maxY_l);
		bool left_l = rand_p.roll(0, 1) > 0;

		// look for extremal value with y = pos_l
		// if left we look for max, otherwise we look for min
		int extremal_l = 0;
		for(TerrainCoord const &coord_l : option_p.cases)
		{
			if(coord_l.y == pos_l)
			{
				if(left_l && coord_l.x > extremal_l)
				{
					extremal_l = coord_l.x;
				}
				if(!left_l && coord_l.x < extremal_l)
				{
					extremal_l = coord_l.x;
				}
			}
		}
		if(left_l)
		{
			++extremal_l;
		}
		else
		{
			++extremal_l;
		}

		option_p.cases.push_back({extremal_l, pos_l});
	}
}

/// create shape of the circle option (cases are in relative position)
void createCircleOption(RandomGenerator &rand_p, TerrainOption &option_p, int minCircleSize_p, int maxCircleSize_p)
{
	int size_l = rand_p.roll(minCircleSize_p, maxCircleSize_p);

	option_p.cases.clear();

	for(int x = -size_l ; x <= size_l ; ++x )
	{
		for(int y = -size_l ; y <= size_l ; ++y )
		{
			if(x*x+y*y <= size_l*size_l)
			{
				option_p.cases.push_back({x,y});
			}
		}
	}
}

/// create shape of the line option (cases are in relative position)
void createLineOption(RandomGenerator &rand_p, TerrainOption &option_p, int minLineSize_p, int maxLineSize_p)
{
	int size_l = rand_p.roll(minLineSize_p, maxLineSize_p);
	// If firection is true the line will be vertical (along y axis)
	bool direction_l = rand_p.roll(0, 1) > 0;

	option_p.cases.clear();

	for(int x = 0 ; x < size_l ; ++x )
	{
		if(direction_l)
		{
			option_p.cases.push_back({0,x});
		}
		else
		{
			option_p.cases.push_back({x,0});
		}
	}
}

void createBrokenLineOption(RandomGenerator &rand_p, TerrainOption &option_p, int minLineBrokenSize_p, int maxLineBrokenSize_p)
{
	int size_l = rand_p.roll(minLineBrokenSize_p, maxLineBrokenSize_p);
	// If firection is true the line will be vertical (along y axis)
	bool direction_l = rand_p.roll(0, 1) > 0;

	option_p.cases.clear();

	for(int x = 0 ; x < size_l ; ++x )
	{
		if(direction_l)
		{
			option_p.cases.push_back({0,-x});
		}
		else
		{
			option_p.cases.push_back({-x,0});
		}
	}

	for(int x = 0 ; x < size_l ; ++x )
	{
		if(direction_l)
		{
			option_p.cases.push_back({1,x});
		}
		else
		{
			option_p.cases.push_back({x,1});
		}
	}
}

} // octopus
