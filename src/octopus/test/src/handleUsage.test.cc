#include <gtest/gtest.h>

#include <controller/Controller.hh>
#include <controller/trigger/Listener.hh>
#include <controller/trigger/Trigger.hh>
#include <state/State.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/command/flying/FlyingCommandSpawnStep.hh>
#include <step/entity/buff/EntityBuffStep.hh>
#include <step/entity/EntityHitPointChangeStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <step/player/PlayerSpendResourceStep.hh>

///
/// This test suite aims at checking that Handles are correctly reusable
///

using namespace octopus;

class HandleUsageTestCommand : public CommandEffectOverTime
{
public:
	HandleUsageTestCommand(std::vector<Steppable *> const &steps_p, Handle handle_p, unsigned long delay_p)
		: CommandEffectOverTime(handle_p, delay_p, 1)
		, _steps(steps_p)
	{}

	virtual void applyEffect(Step & step_p, State const &, CommandData const *, PathManager &) const
	{
		for(Steppable * step_l : _steps)
		{
			step_p.addSteppable(step_l);
		}
	}

private:
	std::vector<Steppable *> _steps;
};

TEST(handleUsageTest, reusage)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(0, Entity { { 3, 3. }, false, unitModel_l}),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 0), -10, 10, 10)}, 0, 2)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntitySpawnStep(Handle(0, 1), Entity { { 3, 3. }, false, unitModel_l})}, 1, 4)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 1), -10, 10, 10)}, 2, 6)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 1), 10, 0, 10)}, 3, 8))
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(2.);	// (2)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(1.);	// (3)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(2.);	// (5)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(2.);	// (7)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(5.);	// (9)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 1)));
}

TEST(handleUsageTest, reusage_throw_wrong_handle)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(0, Entity { { 3, 3. }, false, unitModel_l}),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 0), -10, 10, 10)}, 0, 2)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntitySpawnStep(Handle(0, 1), Entity { { 3, 3. }, false, unitModel_l})}, 1, 4)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 0), 10, 0, 10)}, 3, 6))
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(2.);	// (2)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(1.);	// (3)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 1)));

	controller_l.update(2.);	// (5)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_FALSE(state_l->isEntityAlive(Handle(0, 0)));
	EXPECT_TRUE(state_l->isEntityAlive(Handle(0, 1)));

	bool catched_l = false;
	try
	{
		controller_l.update(2.);	// (7)
		while(!controller_l.loop_body()) {}
		state_l = controller_l.queryState();
	}
	catch(const std::exception& e)
	{
		catched_l = true;
	}

	EXPECT_TRUE(catched_l);
}

TEST(handleUsageTest, attack_move_death_replaced_during_attack)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(0, Entity { { 3, 3. }, false, unitModel_l}),
		new EntitySpawnStep(1, Entity { { 11, 3. }, false, unitModel_l}),
		// entity 0 attack entity 1
		new CommandSpawnStep(new EntityAttackCommand(0, 0, 1, true)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(1, 0), -10, 10, 10)}, 0, 4)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntitySpawnStep(Handle(1,1), Entity { { 11, 3. }, false, unitModel_l})}, 1, 6)),
	}, 1);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	// update time to 1second (1)
	controller_l.update(1);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(4., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	// update time to 2 seconds (3)
	controller_l.update(2);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(1)->_hp), 1e-5);

	// update time to 2 seconds (5)
	controller_l.update(2);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// timer damage should have been done
	EXPECT_NEAR(0., to_double(state_l->getEntity(1)->_hp), 1e-5);

	// update time to 1 second (6)
	controller_l.update(1);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// timer damage should have been done
	EXPECT_NEAR(0., to_double(state_l->getEntity(1)->_hp), 1e-5);

	// update time to 20 seconds (26)
	controller_l.update(20);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has not been done since it's a new entity
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(1,1))->_hp), 1e-5);
}

TEST(handleUsageTest, attack_move_death_replaced_during_move)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(0, Entity { { 3, 3. }, false, unitModel_l}),
		new EntitySpawnStep(1, Entity { { 11, 3. }, false, unitModel_l}),
		// entity 0 attack entity 1
		new CommandSpawnStep(new EntityAttackCommand(0, 0, 1, true)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(1, 0), -10, 10, 10)}, 0, 1)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntitySpawnStep(Handle(1,1), Entity { { 11, 3. }, false, unitModel_l})}, 1, 4)),
	}, 1);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	// update time to 1second (1)
	controller_l.update(1);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(4., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	// update time to 2 seconds (3)
	controller_l.update(2);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// entity 0 stopped because entity was killed
	EXPECT_NEAR(5., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);
	EXPECT_NEAR(0., to_double(state_l->getEntity(1)->_hp), 1e-5);

	// update time to 20 seconds (23)
	controller_l.update(20);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.y), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(1,1))->_hp), 1e-5);

}

class OnEachTriggerResourceTest : public OnEachTrigger
{
public:
	OnEachTriggerResourceTest(Listener * listener_p) : OnEachTrigger(listener_p) {}

	virtual void trigger(State const &, Step &step_p, unsigned long, TriggerData const &) const override
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.;
		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};


///
/// > 1 entities
/// > trigger on set of one of the unit : add ressource to player to check
/// > check that trigger does not trigger twice when the unit is replaced
///
TEST(handleUsageTest, trigger_unit_died)
{
	EntityModel entityModel_l { false, 1., 1., 10. };

	Entity unit_l({1,3}, false, entityModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(0, unit_l),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 0), -10, 10, 10)}, 0, 2)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntitySpawnStep(Handle(0,1), Entity { { 11, 3. }, false, entityModel_l})}, 1, 4)),
		new FlyingCommandSpawnStep(new HandleUsageTestCommand({new EntityHitPointChangeStep(Handle(0, 1), -10, 10, 10)}, 2, 6)),
	}, 1.);

	controller_l.commitTrigger(new OnEachTriggerResourceTest(new ListenerEntityDied({0})));

	// query state
	State const * state_l = controller_l.queryState();

	controller_l.update(3.);	// (3)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// at this point damage should be dealt
	EXPECT_EQ(1u, state_l->getEntities().size());
	EXPECT_NEAR(0., to_double(state_l->getEntity(0)->_hp), 1e-3);
	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	controller_l.update(2.);	// (5)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// at this point damage should be dealt
	EXPECT_EQ(1u, state_l->getEntities().size());
	EXPECT_NEAR(10., to_double(state_l->getEntity({0,1})->_hp), 1e-3);
	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	controller_l.update(10.);	// (15)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// at this point damage should be dealt
	EXPECT_EQ(1u, state_l->getEntities().size());
	EXPECT_NEAR(0., to_double(state_l->getEntity({0,1})->_hp), 1e-3);
	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
}
