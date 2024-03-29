#include <gtest/gtest.h>

#include "logger/Logger.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
#include "controller/Controller.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Building.hh"
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
	Unit unit_l({0,0}, false, unitModel_l);
	Building building_l({0,0}, false, buildingModel_l);

	std::list<AreaSpawn> spawners_l;
	AreaSpawn area_l;
	area_l.x = 2;
	area_l.y = 5;
	area_l.width = 10;
	area_l.height = 10;
	area_l.entities.emplace_back(new Unit(unit_l), 3);
	area_l.entities.emplace_back(new Building(building_l), 3);
	spawners_l.push_back(area_l);

	area_l.x = 2;
	area_l.y = 7;
	area_l.width = 10;
	area_l.height = 10;
	area_l.entities.clear();
	area_l.entities.emplace_back(new Unit(unit_l), 2);
	area_l.entities.emplace_back(new Building(building_l), 1);
	spawners_l.push_back(area_l);

	RandomGenerator gen_p(42);
	AreaSpawnerCommand * spawnCommand_l = new AreaSpawnerCommand(gen_p, spawners_l);
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
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(6., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(1))->_pos.x), 1e-5);
	EXPECT_NEAR(8., to_double(state_l->getEntity(Handle(1))->_pos.y), 1e-5);
	EXPECT_NEAR(3. , to_double(state_l->getEntity(Handle(2))->_pos.x), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(2))->_pos.y), 1e-5);
	EXPECT_NEAR(3. , to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(12., to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);

	/// units
	EXPECT_NEAR(2.5 , to_double(state_l->getEntity(Handle(4))->_pos.x), 1e-5);
	EXPECT_NEAR(13.5, to_double(state_l->getEntity(Handle(4))->_pos.y), 1e-5);
	EXPECT_NEAR(2.5 , to_double(state_l->getEntity(Handle(5))->_pos.x), 1e-5);
	EXPECT_NEAR(13.5, to_double(state_l->getEntity(Handle(5))->_pos.y), 1e-5);
	EXPECT_NEAR(2.5 , to_double(state_l->getEntity(Handle(6))->_pos.x), 1e-5);
	EXPECT_NEAR(13.5, to_double(state_l->getEntity(Handle(6))->_pos.y), 1e-5);
	EXPECT_NEAR(2.5 , to_double(state_l->getEntity(Handle(7))->_pos.x), 1e-5);
	EXPECT_NEAR(13.5, to_double(state_l->getEntity(Handle(7))->_pos.y), 1e-5);
	EXPECT_NEAR(2.5 , to_double(state_l->getEntity(Handle(8))->_pos.x), 1e-5);
	EXPECT_NEAR(13.5, to_double(state_l->getEntity(Handle(8))->_pos.y), 1e-5);
}

Option additionPosition(Option const &option_p)
{
	return Option{option_p.x + 20, option_p.y + 20};
}

TEST(areaSpawnerCommandTest, simple_mirror)
{
	UnitModel unitModel_l { false, 0.5, 1., 10. };
	BuildingModel buildingModel_l { true, 1., 10. };
	Unit unit_l({0,0}, false, unitModel_l);
	Building building_l({0,0}, false, buildingModel_l);

	std::list<AreaSpawn> spawners_l;
	AreaSpawn area_l;
	area_l.x = 2;
	area_l.y = 5;
	area_l.width = 10;
	area_l.height = 10;
	area_l.entities.emplace_back(new Unit(unit_l), 1);
	area_l.entities.emplace_back(new Building(building_l), 1);
	spawners_l.push_back(area_l);

	RandomGenerator gen_p(42);
	AreaSpawnerCommand * spawnCommand_l = new AreaSpawnerCommand(gen_p, spawners_l);
	// for testing purpose
	spawnCommand_l->setNonRandom();
	spawnCommand_l->setReplicationOption(additionPosition);

	Controller controller_l({
			new PlayerSpawnStep(0, 0)
		}, 1., {
			spawnCommand_l
		});

	// query state
	State const * state_l = controller_l.queryState();

	ASSERT_EQ(4u, state_l->getEntities().size());
	/// buildings
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(6., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(23., to_double(state_l->getEntity(Handle(1))->_pos.x), 1e-5);
	EXPECT_NEAR(26., to_double(state_l->getEntity(Handle(1))->_pos.y), 1e-5);

	/// units
	EXPECT_NEAR(2.5 , to_double(state_l->getEntity(Handle(2))->_pos.x), 1e-5);
	EXPECT_NEAR(7.5, to_double(state_l->getEntity(Handle(2))->_pos.y), 1e-5);
	EXPECT_NEAR(22.5 , to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(27.5, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
}
