#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/building/BuildingUnitProductionCommand.hh"
#include "controller/Controller.hh"
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that triggers on unit spawn works properly
///

using namespace octopus;

class OnEachTriggerResourceTest : public OnEachTrigger
{
public:
	OnEachTriggerResourceTest(Listener * listener_p) : OnEachTrigger(listener_p) {}

	virtual void trigger(State const &, Step &step_p, unsigned long) const override
	{
		std::map<ResourceType, double> map_l;
		map_l[ResourceType::Food] = -10.;
		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};

class OneShotTriggerResourceTest : public OneShotTrigger
{
public:
	OneShotTriggerResourceTest(Listener * listener_p) : OneShotTrigger({listener_p}) {}

	virtual void trigger(State const &, Step &step_p, unsigned long) const override
	{
		std::map<ResourceType, double> map_l;
		map_l[ResourceType::Food] = -10.;
		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > 2 entities
///  - building player 0 :  1, 3 production
///  - building player 1 : 10, 3 production
/// > trigger on each unit spawn : add ressource to player to check
///
TEST(triggerUnitSpawnTest, two_players)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building productionPlayer0_l({1,3}, true, buildingModel_l);
	productionPlayer0_l._player = 0;
	Building productionPlayer1_l({10,3}, true, buildingModel_l);
	productionPlayer1_l._player = 1;

	// unit production
	BuildingUnitProductionCommand * command0_l = new BuildingUnitProductionCommand(0, 0, unitModel_l);
	BuildingUnitProductionCommand * command1_l = new BuildingUnitProductionCommand(1, 1, unitModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 0),
		new BuildingSpawnStep(0, productionPlayer0_l, true),
		new BuildingSpawnStep(1, productionPlayer1_l, true),
		new CommandSpawnStep(command0_l),
		new CommandSpawnStep(command1_l)
	}, 1.);

	controller_l.commitTrigger(new OnEachTriggerResourceTest(new ListenerEntityModelFinished(&unitModel_l, 0)));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 7 seconds (7)
	// at this point production time is over and spawned has been done
	// triggered on the step after spawn
	// but trigger step has not been applied yet
	controller_l.update(7.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1second (8)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	// only 10 food because trigger triggered only on 1 player
	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}


///
/// > 2 entities
///  - building player 0 :  1, 3 production
///  - building player 0 : 10, 3 production
/// > trigger on each unit spawn : add ressource to player to check
///
TEST(triggerUnitSpawnTest, two_models)
{
	UnitModel unitModel0_l { false, 1., 1., 10. };
	unitModel0_l._isUnit = true;
	unitModel0_l._productionTime = 5;

	UnitModel unitModel1_l { false, 1., 1., 10. };
	unitModel1_l._isUnit = true;
	unitModel1_l._productionTime = 5;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production0_l({1,3}, true, buildingModel_l);
	Building production1_l({10,3}, true, buildingModel_l);

	// unit production
	BuildingUnitProductionCommand * command0_l = new BuildingUnitProductionCommand(0, 0, unitModel0_l);
	BuildingUnitProductionCommand * command1_l = new BuildingUnitProductionCommand(1, 1, unitModel1_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new BuildingSpawnStep(0, production0_l, true),
		new BuildingSpawnStep(1, production1_l, true),
		new CommandSpawnStep(command0_l),
		new CommandSpawnStep(command1_l)
	}, 1.);

	controller_l.commitTrigger(new OnEachTriggerResourceTest(new ListenerEntityModelFinished(&unitModel0_l, 0)));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 7 seconds (7)
	// at this point production time is over and spawned has been done
	// triggered on the step after spawn
	// but trigger step has not been applied yet
	controller_l.update(7.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1second (8)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	// only 10 food because trigger triggered only on 1 model
	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}


///
/// > 2 entities
///  - building player 0 :  1, 3 production
///  - building player 0 : 10, 3 production
/// > trigger on each unit spawn : add ressource to player to check
///
TEST(triggerUnitSpawnTest, one_model)
{
	UnitModel unitModel0_l { false, 1., 1., 10. };
	unitModel0_l._isUnit = true;
	unitModel0_l._productionTime = 5;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production0_l({1,3}, true, buildingModel_l);
	Building production1_l({10,3}, true, buildingModel_l);

	// unit production
	BuildingUnitProductionCommand * command0_l = new BuildingUnitProductionCommand(0, 0, unitModel0_l);
	BuildingUnitProductionCommand * command1_l = new BuildingUnitProductionCommand(1, 1, unitModel0_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new BuildingSpawnStep(0, production0_l, true),
		new BuildingSpawnStep(1, production1_l, true),
		new CommandSpawnStep(command0_l),
		new CommandSpawnStep(command1_l)
	}, 1.);

	controller_l.commitTrigger(new OnEachTriggerResourceTest(new ListenerEntityModelFinished(&unitModel0_l, 0)));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 7 seconds (7)
	// at this point production time is over and spawned has been done
	// triggered on the step after spawn
	// but trigger step has not been applied yet
	controller_l.update(7.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1second (8)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	// 20 food because trigger triggered on 2 model
	EXPECT_NEAR(20., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}


///
/// > 2 entities
///  - building player 0 :  1, 3 production
///  - building player 0 : 10, 3 production
/// > trigger on each unit spawn : add ressource to player to check
///
TEST(triggerUnitSpawnTest, one_shot)
{
	UnitModel unitModel0_l { false, 1., 1., 10. };
	unitModel0_l._isUnit = true;
	unitModel0_l._productionTime = 5;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production0_l({1,3}, true, buildingModel_l);
	Building production1_l({10,3}, true, buildingModel_l);

	// unit production
	BuildingUnitProductionCommand * command0_l = new BuildingUnitProductionCommand(0, 0, unitModel0_l);
	BuildingUnitProductionCommand * command1_l = new BuildingUnitProductionCommand(1, 1, unitModel0_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new BuildingSpawnStep(0, production0_l, true),
		new BuildingSpawnStep(1, production1_l, true),
		new CommandSpawnStep(command0_l),
		new CommandSpawnStep(command1_l)
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerResourceTest(new ListenerEntityModelFinished(&unitModel0_l, 0)));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 7 seconds (7)
	// at this point production time is over and spawned has been done
	// triggered on the step after spawn
	// but trigger step has not been applied yet
	controller_l.update(7.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1second (8)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
	// 10 food because trigger triggered on 2 model but was one shot
	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}
