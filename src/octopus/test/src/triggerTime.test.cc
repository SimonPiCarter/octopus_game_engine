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
#include "state/entity/Unit.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that trigger on step count work properly
/// Also check that trigger on trigger works correctly
///

using namespace octopus;

class TestTriggerTimeTrigger : public OneShotTrigger
{
public:
	TestTriggerTimeTrigger(std::list<Listener *> const &listeners_p, UnitModel const &model_p) : OneShotTrigger(listeners_p), _model(model_p) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.;

		step_p.addSteppable(state_p, new PlayerSpendResourceStep(0, map_l));

		Unit unit_l({ 5, 5. }, false, _model);
		unit_l._player = 0;
		step_p.addSteppable(state_p, new UnitSpawnStep(getNextHandle(step_p, state_p), unit_l));
	}
private:
	UnitModel const &_model;
};

class TestTriggerTimeSpawnTrigger : public OneShotTrigger
{
public:
	TestTriggerTimeSpawnTrigger(std::list<Listener *> const &listeners_p) : OneShotTrigger(listeners_p) {}

	virtual void trigger(State const & state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.;

		step_p.addSteppable(state_p, new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > no entity
///   > Should spawn resources after a given number of step
///
TEST(triggerTimeTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
	}, 1.);

	controller_l.commitTrigger(new TestTriggerTimeTrigger({new ListenerStepCount(10)}, unitModel_l));
	controller_l.commitTrigger(new TestTriggerTimeSpawnTrigger({new ListenerEntityModelFinished(&unitModel_l, 0)}));

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

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
	EXPECT_EQ(0, state_l->getEntities().size());

	// update time to 1 seconds (12)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
	EXPECT_EQ(1, state_l->getEntities().size());

	// update time to 2 seconds (14)
	// One step to trigger
	// One step to apply the resource change
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(20., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
	EXPECT_EQ(1, state_l->getEntities().size());

	// update time to 10 seconds (24)
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(20., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
	EXPECT_EQ(1, state_l->getEntities().size());
}
