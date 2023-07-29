#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <controller/Controller.hh>
#include <state/State.hh>
#include <state/model/entity/UnitModel.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <step/player/PlayerSpendResourceStep.hh>
#include "command/entity/EntityAbilityCommand.hh"

///
/// This test suite aims at checking that we can have custom abilities
///

using namespace octopus;

// spawn 10 bloc
void customAbility(Step &step_p, State const &, Handle const &, Handle const &, Vector const &)
{
	std::map<std::string, Fixed> map_l;
	map_l["bloc"] = -10.;

	step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
}

TEST(abilityEntityTest, simple)
{
	EntityModel model_l { false, 1., 1., 10. };
	model_l._abilities["spawn_res"] = {"spawn_res", "spawn_res", customAbility};
	model_l._abilityReloadTime["spawn_res"] = 5;

	Entity ent_l { { 3, 3. }, false, model_l};

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(Handle(0), ent_l)
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	controller_l.commitCommand(new EntityAbilityCommand(Handle(0), Handle(0), Vector(), "spawn_res"));
	// update time to 1 seconds (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(0., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	// update time to 1 seconds (2)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	controller_l.commitCommand(new EntityAbilityCommand(Handle(0), Handle(0), Vector(), "spawn_res"));

	// update time to 5 second (7)
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// reload time is not over therefore no resource has been spawned
	EXPECT_NEAR(10., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);

	// only one should work
	controller_l.commitCommand(new EntityAbilityCommand(Handle(0), Handle(0), Vector(), "spawn_res"));
	controller_l.commitCommand(new EntityAbilityCommand(Handle(0), Handle(0), Vector(), "spawn_res"));

	// update time to 2 seconds (9)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// reload time was over therefore no resource has been spawned
	EXPECT_NEAR(20., to_double(getResource(*state_l->getPlayer(0), "bloc")), 1e-5);
}
