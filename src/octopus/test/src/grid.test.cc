#include <gtest/gtest.h>

#include "graph/Grid.hh"
#include "logger/Logger.hh"

using namespace octopus;

TEST(gridTest, simple)
{
	///
	/// Create a path from a grid
	///
	///
	Grid grid_l(10, 10, 1., 1.);
	Graph &graph_l = grid_l.getGraph();

	std::list<GridNode const *> path_l = graph_l.getPath(grid_l.getNode(0,0), grid_l.getNode(9,9));
	trimPath(path_l);

	std::vector<Vector> expected_l = {
		{ 0, 0 },
		{ 9, 9 }
	};
	ASSERT_EQ(expected_l.size(), path_l.size());
	size_t idx_l = 0;
	for(GridNode const * node_l : path_l)
	{
		EXPECT_TRUE(expected_l.at(idx_l) == node_l->getPosition() - Vector(0.5,0.5));// << "expected = "<<expected_l.at(idx_l) << " real = "<<node_l->getPosition();
		++idx_l;
	}

	for(size_t i = 1; i < 8 ; ++ i)
	{
		for(size_t j = 1; j < 8 ; ++ j)
		{
			grid_l.getNode(i,j)->setFree(false);
		}
	}
	path_l = graph_l.getPath(grid_l.getNode(0,0), grid_l.getNode(9,9));
	trimPath(path_l);
	expected_l = {
		{ 0, 0 },
		{ 7, 0 },
		{ 8, 1 },
		{ 8, 8 },
		{ 9, 9 }
	};

	ASSERT_EQ(expected_l.size(), path_l.size());
	idx_l = 0;
	for(GridNode const * node_l : path_l)
	{
		EXPECT_TRUE(expected_l.at(idx_l) == node_l->getPosition() - Vector(0.5,0.5));// << "expected = "<<expected_l.at(idx_l) << " real = "<<node_l->getPosition();
		++idx_l;
	}
}
