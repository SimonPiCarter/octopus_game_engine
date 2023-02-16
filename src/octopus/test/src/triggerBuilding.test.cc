#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/entity/EntityBuildingCommand.hh"
#include "controller/Controller.hh"
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that trigger building
///

using namespace octopus;

class TestTriggerBuildingTrigger : public OneShotTrigger
{
public:
	TestTriggerBuildingTrigger(std::list<Listener *> const &listeners_p) : OneShotTrigger(listeners_p) {}

	virtual void trigger(Step &step_p) const override
	{
		std::map<ResourceType, double> map_l;
		map_l[ResourceType::Food] = -10.;

		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > 2 entities
///  - unit : 5 3 building
///  - building : 1, 3
///
TEST(triggerBuildingTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._isBuilder = true;
	Unit unit_l({ 5, 3. }, false, unitModel_l);

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._buildingTime = 10;

	Building building_l({1,3}, true, depositModel_l);
	building_l._alive = false;

	// unit building command
	EntityBuildingCommand * command_l = new EntityBuildingCommand(0, 0, 1, {1, 3}, 0, {{1, 3}});

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(unit_l),
		new BuildingSpawnStep(building_l, false),
		new CommandSpawnStep(command_l)
	}, 1.);

	std::map<ResourceType, double> map_l;
	map_l[ResourceType::Food] = -10.;

	controller_l.commitTrigger(new TestTriggerBuildingTrigger({new ListenerEntityModelFinished(&depositModel_l, 0)}));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 2 seconds (2)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 10 seconds (12)
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1 seconds (13)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}
