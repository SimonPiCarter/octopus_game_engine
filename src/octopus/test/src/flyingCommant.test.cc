#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/flying/CommandEffectOverTime.hh"
#include "controller/Controller.hh"
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that trigger on step count work properly
/// Also check that trigger on trigger works correctly
///

using namespace octopus;

class TestResourceOverTime : public CommandEffectOverTime
{
public:
	TestResourceOverTime(unsigned long tickRate_p, unsigned long nbOfTicks_p)
		: CommandEffectOverTime(tickRate_p, nbOfTicks_p) {}


	virtual void applyEffect(Step & step_p, State const &, CommandData const *, PathManager &) const
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.;

		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
	}
};

///
/// > no entity
///   > Should spawn resources after a given number of step
///
TEST(flyingCommantTest, simple)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new FlyingCommandSpawnStep(new TestResourceOverTime(10, 2)),
	}, 1.);


	// query state
	State const * state_l = controller_l.queryState();

	// update time to 10 seconds (10)
	// 10 step to trigger
	// trigger step has been spawned -> not applied yet
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 1 seconds (11)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 9 seconds (20)
	// One step to trigger effect
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 1 seconds (21)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(20., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);

	// update time to 100 seconds (121)
	controller_l.update(100.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(20., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-3);
}
