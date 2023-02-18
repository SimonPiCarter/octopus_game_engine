#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <controller/Controller.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerAddOptionDivinityStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <command/player/PlayerChoseDivinityCommand.hh>
#include <state/State.hh>
#include <state/player/Player.hh>

///
/// This test suite aims at checking that Divinity options behave correctly
///

using namespace octopus;

TEST(divinityCommandTest, simple_ko)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_1, DivinityType::Divinity_2}, false)
	}, 1.);

    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_3));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(0, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_3));
	EXPECT_NEAR(0., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_3, true), 1e-5);
}

TEST(divinityCommandTest, simple_ok)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_1, DivinityType::Divinity_2}, false)
	}, 1.);

    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_1));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(0, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_1));
	EXPECT_NEAR(120., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_1, true), 1e-5);
}

TEST(divinityCommandTest, simple_lvlup_ko)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_1, DivinityType::Divinity_2}, true)
	}, 1.);

    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_3));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(0, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_3));
	EXPECT_NEAR(0., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_3, true), 1e-5);
}

TEST(divinityCommandTest, simple_lvlup_ok)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_1, DivinityType::Divinity_2}, true)
	}, 1.);

    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_1));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_1));
	EXPECT_NEAR(120., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_1, true), 1e-5);
}

TEST(divinityCommandTest, simple_lvlup_two_same_step)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_1, DivinityType::Divinity_2}, true)
	}, 1.);

    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_1));
    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_1));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_1));
	EXPECT_NEAR(120., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_1, true), 1e-5);
}

TEST(divinityCommandTest, simple_lvlup_three_same_step_diff_options)
{
	Controller controller_l({
		new PlayerSpawnStep(0, 0),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_1, DivinityType::Divinity_2}, true),
        new PlayerAddOptionDivinityStep(0, {DivinityType::Divinity_3}, true)
	}, 1.);

    // Consume last options first
    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_3));
    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_1));
    controller_l.commitCommand(new PlayerChoseDivinityCommand(0, DivinityType::Divinity_1));

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_1));
	EXPECT_NEAR(120., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_1, true), 1e-5);
	EXPECT_EQ(1, getDivLvl(*state_l->getPlayer(0), DivinityType::Divinity_3));
	EXPECT_NEAR(120., getDivAnchor(*state_l->getPlayer(0), DivinityType::Divinity_3, true), 1e-5);
}
