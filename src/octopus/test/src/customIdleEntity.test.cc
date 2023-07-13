#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <controller/Controller.hh>
#include <state/State.hh>
#include <state/model/entity/UnitModel.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <step/player/PlayerSpendResourceStep.hh>
#include "step/entity/EntityUpdateWaitingStep.hh"

///
/// This test suite aims at checking that we can have idle custom command
///

using namespace octopus;

// spawn 10 bloc every 5 steps
void customIdle(Entity const &ent_p, Step &step_p, State const &, CommandContext &)
{
	if(ent_p._waiting >= 5)
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.;

		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
	}
}

TEST(customIdleEntityTest, simple)
{
	EntityModel model_l { false, 1., 1., 10. };
	model_l._idleFunc = customIdle;

	Entity ent1_l { { 3, 3. }, false, model_l};

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(0, ent1_l)
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	// update time to 1 seconds (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// waiting time starts at 10000 therefore we instatly spawn resource
	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	// update time to 5 seconds (6)
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// next tick should not have happened yet
	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	// update time to 1 second (7)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// next tick should have happened now
	EXPECT_NEAR(20., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);
}
