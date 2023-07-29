#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/flying/eot/DamageOverTime.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/bonus/PlayerAddTimeProdBonusStep.hh"
#include "step/player/bonus/PlayerAddCostBonusStep.hh"

///
/// This test suite aims at checking that production command works properly with bonuses
///

using namespace octopus;

///
/// > 1 entity
///  - building : production
///
TEST(unitProductionBonusTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;


	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(Handle(0), production_l, true);

	// unit production
	BuildingUnitProductionCommand * command_l = new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, commandSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 5second (5)
	// at this point production time is over but no spawn yet
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1second (6)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());
}

///
/// > 1 entity
///  - building : production
/// prod time bonus flat
///
TEST(unitProductionBonusTest, simple_time_flat)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;
	unitModel_l._id = "model";


	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(Handle(0), production_l, true);

	// unit production
	BuildingUnitProductionCommand * command_l = new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Steppable * bonus_l = new PlayerAddTimeProdBonusStep(0, "model", Buff{-2, 0.});

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, commandSpawn_l, bonus_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time 3 seconds (5)
	// at this point production time is over but no spawn yet
	controller_l.update(3.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time 1 second (6)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());
}

///
/// > 1 entity
///  - building : production
/// prod time bonus coef
///
TEST(unitProductionBonusTest, simple_time_coef)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;
	unitModel_l._id = "model";

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(Handle(0), production_l, true);

	// unit production
	BuildingUnitProductionCommand * command_l = new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Steppable * bonus_l = new PlayerAddTimeProdBonusStep(0, "model", Buff{0, 0.4});

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, commandSpawn_l, bonus_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 7second (7)
	// at this point production time is over but no spawn yet
	controller_l.update(7.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1second (8)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());
}

///
/// > 1 entity
///  - building : production
/// cost bonus flat
///
TEST(unitProductionBonusTest, simple_cost_flat)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;
	unitModel_l._id = "model";
	unitModel_l._cost["bloc"] = 100;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._unitModels.push_back(&unitModel_l);

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(Handle(0), production_l, true);

	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -100;

	// unit production
	BuildingUnitProductionCommand * command_l = new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l);

	Steppable * bonus_l = new PlayerAddCostBonusStep(0, "model", "bloc", Buff{-20, 0.});

	Controller controller_l({new PlayerSpawnStep(0, 0), new PlayerSpendResourceStep(0, mapRes_l), spawn0_l, bonus_l}, 1.);
	controller_l.commitCommand(command_l);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1second (1)
	// at this point production should have started and cost be spent
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());
	EXPECT_NEAR(20., getResource(*state_l->getPlayer(0), "bloc").to_double(), 1e-5);
}

///
/// > 1 entity
///  - building : production
/// cost bonus coef
///
TEST(unitProductionBonusTest, simple_cost_coef)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;
	unitModel_l._id = "model";
	unitModel_l._cost["bloc"] = 100;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._unitModels.push_back(&unitModel_l);

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(Handle(0), production_l, true);

	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -100;

	// unit production
	BuildingUnitProductionCommand * command_l = new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l);

	Steppable * bonus_l = new PlayerAddCostBonusStep(0, "model", "bloc", Buff{0., -0.2});

	Controller controller_l({new PlayerSpawnStep(0, 0), new PlayerSpendResourceStep(0, mapRes_l), spawn0_l, bonus_l}, 1.);
	controller_l.commitCommand(command_l);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1second (1)
	// at this point production should have started and cost be spent
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());
	EXPECT_NEAR(20., getResource(*state_l->getPlayer(0), "bloc").to_double(), 1e-5);
}
