#include <gtest/gtest.h>

#include "logger/Logger.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
#include "controller/Controller.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/player/PlayerSpawnStep.hh"

///
/// This test suite aims at checking that AreaSpawnerCommand works
/// correctly
///

using namespace octopus;

TEST(areaSpawnerCommandTest, simple)
{
	UnitModel unitModel_l { false, 0.5, 1., 10. };
	BuildingModel buildingModel_l { true, 1., 10. };

	std::list<AreaSpawn> spawners_l;
	AreaSpawn area_l;
	area_l.x = 2;
	area_l.y = 5;
	area_l.size = 10;
	area_l.models.emplace_back(&unitModel_l, 3);
	area_l.models.emplace_back(&buildingModel_l, 3);
	spawners_l.push_back(area_l);

	area_l.x = 2;
	area_l.y = 7;
	area_l.size = 10;
	area_l.models.clear();
	area_l.models.emplace_back(&unitModel_l, 2);
	area_l.models.emplace_back(&buildingModel_l, 1);
	spawners_l.push_back(area_l);

	AreaSpawnerCommand * spawnCommand_l = new AreaSpawnerCommand(spawners_l);
	// for testing purpose
	spawnCommand_l->setNonRandom();

	Controller controller_l({
			new PlayerSpawnStep(0, 0)
		}, 1., {
			spawnCommand_l
		});

	// query state
	State const * state_l = controller_l.queryState();

	ASSERT_EQ(9u, state_l->getEntities().size());
	/// buildings
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(6., state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(1)->_pos.x, 1e-5);
	EXPECT_NEAR(8., state_l->getEntity(1)->_pos.y, 1e-5);
	EXPECT_NEAR(3. , state_l->getEntity(2)->_pos.x, 1e-5);
	EXPECT_NEAR(10., state_l->getEntity(2)->_pos.y, 1e-5);
	EXPECT_NEAR(3. , state_l->getEntity(3)->_pos.x, 1e-5);
	EXPECT_NEAR(12., state_l->getEntity(3)->_pos.y, 1e-5);

	/// units
	EXPECT_NEAR(2.5 , state_l->getEntity(4)->_pos.x, 1e-5);
	EXPECT_NEAR(13.5, state_l->getEntity(4)->_pos.y, 1e-5);
	EXPECT_NEAR(2.5 , state_l->getEntity(5)->_pos.x, 1e-5);
	EXPECT_NEAR(13.5, state_l->getEntity(5)->_pos.y, 1e-5);
	EXPECT_NEAR(2.5 , state_l->getEntity(6)->_pos.x, 1e-5);
	EXPECT_NEAR(13.5, state_l->getEntity(6)->_pos.y, 1e-5);
	EXPECT_NEAR(2.5 , state_l->getEntity(7)->_pos.x, 1e-5);
	EXPECT_NEAR(13.5, state_l->getEntity(7)->_pos.y, 1e-5);
	EXPECT_NEAR(2.5 , state_l->getEntity(8)->_pos.x, 1e-5);
	EXPECT_NEAR(13.5, state_l->getEntity(8)->_pos.y, 1e-5);

}
