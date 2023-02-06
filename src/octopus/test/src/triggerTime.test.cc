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
/// This test suite aims at checking that trigger on step count work properly
///

using namespace octopus;

class TestTriggerTimeTrigger : public OneShotTrigger
{
public:
	TestTriggerTimeTrigger(std::list<Listener *> const &listeners_p) : OneShotTrigger(listeners_p) {}

	virtual void trigger(Step &step_p) const override
	{
		std::map<ResourceType, double> map_l;
		map_l[ResourceType::Food] = -10.;

		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > no entity
///   > Should spawn resources after a given number of step
///
TEST(triggerTimeTest, simple)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
	}, 1.);

	controller_l.commitTrigger(new TestTriggerTimeTrigger({new ListenerStepCount(10)}));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 11 seconds (11)
	// 1 step to spawn trigger
	// 10 step to trigger
	// trigger step has been spawned -> not applied yet
	controller_l.update(11.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1 seconds (12)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 11 seconds (23)
	controller_l.update(11.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}
