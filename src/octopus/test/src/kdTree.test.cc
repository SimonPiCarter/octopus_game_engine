#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <state/model/entity/EntityModel.hh>
#include <state/entity/Entity.hh>
#include <utils/KdTree.hh>

///
/// This test suite aims at checking that KdTree works properly
///

using namespace octopus;

///
/// > 5 entities
///  - 3,3
///  - 4,3
///  - 5,3
///  - 3,4.1
///  - 3,5.1
///
class KdTreeTest : public ::testing::Test
{
public:
	KdTreeTest()
	{
		EntityModel entityModel_l { false, 1., 1., 10. };

		Entity * ent0_l = new Entity { { 3, 3 }, false, entityModel_l}; ent0_l->_handle = 0; ent0_l->_player = 1;
		Entity * ent1_l = new Entity { { 4, 3 }, false, entityModel_l}; ent1_l->_handle = 1; ent1_l->_player = 2;
		Entity * ent2_l = new Entity { { 5, 3 }, false, entityModel_l}; ent2_l->_handle = 2; ent2_l->_player = 1;
		Entity * ent3_l = new Entity { { 3, 4.1 }, false, entityModel_l}; ent3_l->_handle = 3; ent3_l->_player = 2;
		Entity * ent4_l = new Entity { { 3, 5.1 }, false, entityModel_l}; ent4_l->_handle = 4; ent4_l->_player = 1;

		_entities = {
			ent0_l, ent1_l, ent2_l, ent3_l, ent4_l
		};
	}

	~KdTreeTest()
	{
		for(Entity const * ent_l : _entities)
		{
			delete ent_l;
		}
	}

	///
	std::vector<std::pair<Fixed , const Entity *> > setUpKdTreeTest(Handle const &handle_p, Fixed const &range_p,
		size_t max_p, std::function<bool(Entity const *)> const &fn_p)
	{
		KdTree tree_l(_entities);
		tree_l.buildAgentTree();

		return tree_l.computeEntityNeighbors(handle_p, range_p, max_p, fn_p);
	}

protected:
	std::vector<Entity *> _entities;
};

/// Range is too short no entity is found (range is non inclusive)
TEST_F(KdTreeTest, simple_short_range)
{
	std::vector<std::pair<Fixed , const Entity *> > closest_l = setUpKdTreeTest(0, 1, 4, [](Entity const *) { return true; });

	ASSERT_EQ(0u, closest_l.size());
}

/// Range is just enough to capture 1 entity
TEST_F(KdTreeTest, simple_1_1_range)
{
	std::vector<std::pair<Fixed , const Entity *> > closest_l = setUpKdTreeTest(0, 1.05, 4, [](Entity const *) { return true; });

	ASSERT_EQ(1u, closest_l.size());
	EXPECT_EQ(1u, closest_l[0].second->_handle);
}

/// Range is enough to capture all entities
TEST_F(KdTreeTest, simple_2_2_range)
{
	std::vector<std::pair<Fixed , const Entity *> > closest_l = setUpKdTreeTest(0, 2.2, 4, [](Entity const *) { return true; });

	ASSERT_EQ(4u, closest_l.size());
	EXPECT_EQ(1u, closest_l[0].second->_handle);
	EXPECT_EQ(3u, closest_l[1].second->_handle);
	EXPECT_EQ(2u, closest_l[2].second->_handle);
	EXPECT_EQ(4u, closest_l[3].second->_handle);
}

/// Range is enough to capture all entities but we only want the closest
TEST_F(KdTreeTest, simple_2_2_range_max_1)
{
	std::vector<std::pair<Fixed , const Entity *> > closest_l = setUpKdTreeTest(0, 2.2, 1, [](Entity const *) { return true; });

	ASSERT_EQ(1u, closest_l.size());
	EXPECT_EQ(1u, closest_l[0].second->_handle);
}

/// Range is enough to capture all entities but we only want player 1 (ent 2 and 4)
TEST_F(KdTreeTest, simple_2_2_range_player_1)
{
	std::vector<std::pair<Fixed , const Entity *> > closest_l = setUpKdTreeTest(0, 2.2, 4, [](Entity const *ent_p) { return ent_p->_player == 1; });

	ASSERT_EQ(2u, closest_l.size());
	EXPECT_EQ(2u, closest_l[0].second->_handle);
	EXPECT_EQ(4u, closest_l[1].second->_handle);
}
