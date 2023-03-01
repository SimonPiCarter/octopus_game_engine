#include <gtest/gtest.h>

#include "graph/CanonicalDijkstra.hh"
#include "graph/Grid.hh"

using namespace octopus;

TEST(canonicalTest, empty)
{
	///
	/// Create a path from a grid
	/// (empty)
	///
	Grid grid_l(10, 10, 1., 1., true);
	ValueGrid val_l = canonical_dijkstra(grid_l.getInternalGrid(), 3, 3);

	EXPECT_NEAR(0, val_l[3][3].to_double(), 1e-5);
	EXPECT_NEAR(1., val_l[4][3].to_double(), 1e-5);
	EXPECT_NEAR(1., val_l[3][4].to_double(), 1e-5);
	//stream(std::cout, val_l);
	EXPECT_NEAR(2., val_l[4][4].to_double(), 1e-5);
	EXPECT_NEAR(12., val_l[9][9].to_double(), 1e-5);
	EXPECT_NEAR(10, val_l[8][8].to_double(), 1e-5);
}

TEST(canonicalTest, borders)
{
	///
	/// Create a path from a grid
	/// (only borders are free)
	///
	Grid grid_l(10, 10, 1., 1., true);

	for(size_t i = 1; i < 9 ; ++ i)
	{
		for(size_t j = 1; j < 9 ; ++ j)
		{
			grid_l.getNode(i,j)->setFree(false);
		}
	}

	ValueGrid val_l = canonical_dijkstra(grid_l.getInternalGrid(), 0, 0);

	EXPECT_NEAR(-1., val_l[3][3].to_double(), 1e-5);
	EXPECT_NEAR(-1, val_l[8][8].to_double(), 1e-5);
	EXPECT_NEAR(18, val_l[9][9].to_double(), 1e-5);

	//stream(std::cout, val_l);
}

TEST(canonicalTest, big_50)
{
	Grid grid_l(50, 50, 1., 1., true);
	ValueGrid val_l = canonical_dijkstra(grid_l.getInternalGrid(), 0, 0);
}

TEST(canonicalTest, big_250)
{
	Grid grid_l(250, 250, 1., 1., true);
	ValueGrid val_l = canonical_dijkstra(grid_l.getInternalGrid(), 0, 0);
}

TEST(canonicalTest, big_500)
{
	Grid grid_l(500, 500, 1., 1., true);
	ValueGrid val_l = canonical_dijkstra(grid_l.getInternalGrid(), 0, 0);
}
