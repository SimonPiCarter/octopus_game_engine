#include <gtest/gtest.h>

#include "command/spawner/SpawningGrid.hh"
#include "graph/Grid.hh"

using namespace octopus;

TEST(spawner, options_2)
{
	SpawningGrid grid_l(4);

	std::vector<Option> vec_l = getOptions(grid_l, 2);

	ASSERT_EQ(9, vec_l.size());
}

TEST(spawner, options_2_subgrid)
{
	SpawningGrid grid_l(4);

	std::vector<Option> vec_l = getOptions(1, 1, 3, 3, grid_l, 2);

	ASSERT_EQ(4, vec_l.size());
}

TEST(spawner, options_2_subgrid_irregular)
{
	SpawningGrid grid_l(4);

	std::vector<Option> vec_l = getOptions(1, 0, 3, 4, grid_l, 2);

	ASSERT_EQ(6, vec_l.size());
}

TEST(spawner, options_3)
{
	SpawningGrid grid_l(4);

	std::vector<Option> vec_l = getOptions(grid_l, 3);

	ASSERT_EQ(4, vec_l.size());
}

TEST(spawner, options_4)
{
	SpawningGrid grid_l(4);

	std::vector<Option> vec_l = getOptions(grid_l, 4);

	ASSERT_EQ(1, vec_l.size());
}

TEST(spawner, options_2_few_nodes_not_free_1)
{
	SpawningGrid grid_l(4);

	grid_l.fillGrid(0, 0, 1);

	std::vector<Option> vec_l = getOptions(grid_l, 2);

	ASSERT_EQ(8, vec_l.size());
}

TEST(spawner, options_2_few_nodes_not_free_2)
{
	SpawningGrid grid_l(4);

	grid_l.fillGrid(0, 0, 2);

	std::vector<Option> vec_l = getOptions(grid_l, 2);

	ASSERT_EQ(5, vec_l.size());
}

TEST(spawner, init_grid)
{
	SpawningGrid grid_l(4);

	Grid worldGrid_l(4, 4, 1, 1);
	worldGrid_l.getNode(0, 0)->setFree(false);
	worldGrid_l.getNode(0, 1)->setFree(false);
	worldGrid_l.getNode(1, 0)->setFree(false);

	initGrid(0, 0, grid_l, worldGrid_l);

	std::vector<Option> vec_l = getOptions(grid_l, 2);

	ASSERT_EQ(6, vec_l.size());
}

TEST(spawner, init_grid_relative)
{
	SpawningGrid grid_l(4);

	Grid worldGrid_l(50, 50, 1, 1);
	worldGrid_l.getNode(10, 0)->setFree(false);
	worldGrid_l.getNode(10, 1)->setFree(false);
	worldGrid_l.getNode(11, 0)->setFree(false);

	initGrid(10, 0, grid_l, worldGrid_l);

	std::vector<Option> vec_l = getOptions(grid_l, 2);

	ASSERT_EQ(6, vec_l.size());
}

TEST(spawner, init_grid_relative_2)
{
	SpawningGrid grid_l(4);

	Grid worldGrid_l(50, 50, 1, 1);
	worldGrid_l.getNode(10, 0)->setFree(false);
	worldGrid_l.getNode(10, 1)->setFree(false);
	worldGrid_l.getNode(11, 0)->setFree(false);

	initGrid(11, 0, grid_l, worldGrid_l);

	std::vector<Option> vec_l = getOptions(grid_l, 2);

	ASSERT_EQ(8, vec_l.size());
}
