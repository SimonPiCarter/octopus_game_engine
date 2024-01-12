#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/entity/EntityMoveCommand.hh"
#include "controller/Controller.hh"
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that triggers on zone works correctly
///

using namespace octopus;

class OneShotTriggerZoneTest : public OneShotTrigger
{
public:
	OneShotTriggerZoneTest(Listener * listener_p) : OneShotTrigger({listener_p}) {}

	virtual void trigger(State const & state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.;
		step_p.addSteppable(state_p, new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > 1 entity
/// > trigger we the unit enter a zone
///
TEST(triggerZoneTest, one_unit_right_player)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Unit unit_l({10,2}, false, unitModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(Handle(0), unit_l),
		new CommandSpawnStep(new EntityMoveCommand(Handle(0), Handle(0), {0, 2}, 0, { {0, 2}})),
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerZoneTest(ListenerZone::newListenerZonePlayer(0, Box<long> {0,5,0,5})));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1 seconds
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 10 seconds
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
}

///
/// > 1 entity
/// > trigger we the unit enter a zone
///
TEST(triggerZoneTest, one_unit_wrong_player)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Unit unit_l({10,2}, false, unitModel_l);
	unit_l._player = 1;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 0),
		new UnitSpawnStep(Handle(0), unit_l),
		new CommandSpawnStep(new EntityMoveCommand(Handle(0), Handle(0), {0, 2}, 0, { {0, 2}})),
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerZoneTest(ListenerZone::newListenerZonePlayer(0, Box<long> {0,5,0,5})));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1 seconds
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 10 seconds
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
}

///
/// > 1 entity
/// > trigger we the unit enter a zone
///
TEST(triggerZoneTest, one_unit_right_team)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Unit unit_l({10,2}, false, unitModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(Handle(0), unit_l),
		new CommandSpawnStep(new EntityMoveCommand(Handle(0), Handle(0), {0, 2}, 0, { {0, 2}})),
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerZoneTest(ListenerZone::newListenerZoneTeam(0, Box<long> {0,5,0,5})));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1 seconds
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 10 seconds
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
}

///
/// > 1 entity
/// > trigger we the unit enter a zone
///
TEST(triggerZoneTest, one_unit_wrong_team)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;

	Unit unit_l({10,2}, false, unitModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(Handle(0), unit_l),
		new CommandSpawnStep(new EntityMoveCommand(Handle(0), Handle(0), {0, 2}, 0, { {0, 2}})),
	}, 1.);

	controller_l.commitTrigger(new OneShotTriggerZoneTest(ListenerZone::newListenerZoneTeam(1, Box<long> {0,5,0,5})));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1 seconds
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 10 seconds
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
}
