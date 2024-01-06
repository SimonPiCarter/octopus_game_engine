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

	EXPECT_NEAR(0, to_double(val_l[3][3]), 1e-5);
	EXPECT_NEAR(1., to_double(val_l[4][3]), 1e-5);
	EXPECT_NEAR(1., to_double(val_l[3][4]), 1e-5);
	EXPECT_NEAR(2., to_double(val_l[4][4]), 1e-5);
	EXPECT_NEAR(12., to_double(val_l[9][9]), 1e-5);
	EXPECT_NEAR(10, to_double(val_l[8][8]), 1e-5);
}

TEST(canonicalTest, empty_computation)
{
	///
	/// Create a path from a grid
	/// (empty)
	///
	Grid grid_l(10, 10, 1., 1., true);
	FlowFieldComputation computer_l(grid_l.getInternalGrid(), 3, 3);

	EXPECT_FALSE(computer_l.over());

	EXPECT_EQ(0, computer_l.compute(5));
	EXPECT_FALSE(computer_l.over());

	EXPECT_EQ(0, computer_l.compute(50));
	EXPECT_FALSE(computer_l.over());

	EXPECT_EQ(0, computer_l.compute(50));
	EXPECT_FALSE(computer_l.over());

	EXPECT_EQ(33, computer_l.compute(50));
	EXPECT_TRUE(computer_l.over());

	ValueGrid &val_l = computer_l.valueGrid;

	EXPECT_NEAR(0, to_double(val_l[3][3]), 1e-5);
	EXPECT_NEAR(1., to_double(val_l[4][3]), 1e-5);
	EXPECT_NEAR(1., to_double(val_l[3][4]), 1e-5);
	EXPECT_NEAR(2., to_double(val_l[4][4]), 1e-5);
	EXPECT_NEAR(12., to_double(val_l[9][9]), 1e-5);
	EXPECT_NEAR(10, to_double(val_l[8][8]), 1e-5);
	//stream(std::cout, val_l);
}

TEST(canonicalTest, borders)
{
	///
	/// Create a path from a grid
	/// (only borders are free)
	///
	Grid grid_l(10, 10, 1., 1., true);

	for(uint32_t i = 1; i < 9 ; ++ i)
	{
		for(uint32_t j = 1; j < 9 ; ++ j)
		{
			grid_l.getNode(i,j)->setFree(false);
		}
	}

	ValueGrid val_l = canonical_dijkstra(grid_l.getInternalGrid(), 0, 0);

	EXPECT_NEAR(-1., to_double(val_l[3][3]), 1e-5);
	EXPECT_NEAR(-1, to_double(val_l[8][8]), 1e-5);
	EXPECT_NEAR(18, to_double(val_l[9][9]), 1e-5);

	//stream(std::cout, val_l);

	FlowField field_l = flow_field(val_l);

	//stream(std::cout, field_l);
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
	FlowField field_l = flow_field(val_l);
}
