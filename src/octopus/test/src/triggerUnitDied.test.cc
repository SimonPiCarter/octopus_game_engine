#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/flying/eot/DamageOverTime.hh"
#include "controller/Controller.hh"
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that triggers on unit dying works properly
///

using namespace octopus;

class OneShotTriggerResourceTest : public OneShotTrigger
{
public:
	OneShotTriggerResourceTest(Listener * listener_p) : OneShotTrigger({listener_p}) {}

	virtual void trigger(State const &, Step &step_p, unsigned long, TriggerData const &) const override
	{
		std::map<ResourceType, double> map_l;
		map_l[ResourceType::Food] = -10.;
		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > 2 entities
/// > trigger on set of one of the unit : add ressource to player to check
///
TEST(triggerUnitDiedTest, one_unit)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Unit unit_l({1,3}, false, unitModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(0, unit_l),
		new UnitSpawnStep(1, unit_l),
		new FlyingCommandSpawnStep(new DamageOverTime(0, 5, 2, 5, 0)),
		new FlyingCommandSpawnStep(new DamageOverTime(1, 10, 2, 5, 1)),
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerResourceTest(new ListenerEntityDied({0})));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 10 seconds (10)
	// at this point damage should not be dealt yet
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1second (11)
	// damage should be dealt but trigger not applied yet
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 2second (13)
	// trigger should be applied
	// one step to trigger
	// one step to apply the resource change
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}

///
/// > 2 entities
/// > trigger on set of the two units : add ressource to player to check
///
TEST(triggerUnitDiedTest, two_units)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Unit unit_l({1,3}, false, unitModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(0, unit_l),
		new UnitSpawnStep(1, unit_l),
		new FlyingCommandSpawnStep(new DamageOverTime(0, 5, 2, 5, 0)),
		new FlyingCommandSpawnStep(new DamageOverTime(1, 10, 2, 5, 1)),
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerResourceTest(new ListenerEntityDied({0,1})));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 10 seconds (10)
	// at this point damage should not be dealt yet
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 1second (11)
	// damage should be dealt but trigger not applied yet
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 2second (13)
	// trigger should be applied
	// one step to trigger
	// one step to apply the resource change
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(5., state_l->getEntity(1)->_hp, 1e-3);
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 8second (21)
	controller_l.update(8.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(0., state_l->getEntity(1)->_hp, 1e-3);
	EXPECT_NEAR(0., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);

	// update time to 2second (23)
	// trigger should be applied
	// one step to trigger
	// one step to apply the resource change
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., state_l->getEntity(0)->_hp, 1e-3);
	EXPECT_NEAR(0., state_l->getEntity(1)->_hp, 1e-3);
	EXPECT_NEAR(10., getResource(*state_l->getPlayer(0), ResourceType::Food), 1e-3);
}
