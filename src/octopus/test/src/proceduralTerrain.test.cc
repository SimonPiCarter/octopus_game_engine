#include <gtest/gtest.h>

#include <command/spawner/ProceduralTerrain.hh>
#include <utils/RandomGenerator.hh>

///
/// This test suite aims at checking that procedural terrain works correctly
///

using namespace octopus;

TEST(proceduralTerrainTest, line)
{
	TerrainOption option_l;
	RandomGenerator rand_l(42, true);

	createLineOption(rand_l, option_l, 3, 3);

	EXPECT_EQ(3u, option_l.cases.size());
	EXPECT_EQ(0, option_l.cases.at(0).x);
	EXPECT_EQ(0, option_l.cases.at(0).y);
	EXPECT_EQ(1, option_l.cases.at(1).x);
	EXPECT_EQ(0, option_l.cases.at(1).y);
	EXPECT_EQ(2, option_l.cases.at(2).x);
	EXPECT_EQ(0, option_l.cases.at(2).y);
}

TEST(proceduralTerrainTest, broken_line)
{
	TerrainOption option_l;
	RandomGenerator rand_l(42, true);

	createBrokenLineOption(rand_l, option_l, 2, 2);

	EXPECT_EQ(4u, option_l.cases.size());
	EXPECT_EQ(0, option_l.cases.at(0).x);
	EXPECT_EQ(0, option_l.cases.at(0).y);
	EXPECT_EQ(-1, option_l.cases.at(1).x);
	EXPECT_EQ(0, option_l.cases.at(1).y);
	EXPECT_EQ(0, option_l.cases.at(2).x);
	EXPECT_EQ(1, option_l.cases.at(2).y);
	EXPECT_EQ(1, option_l.cases.at(3).x);
	EXPECT_EQ(1, option_l.cases.at(3).y);
}
