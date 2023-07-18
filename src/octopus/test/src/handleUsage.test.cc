#include <gtest/gtest.h>

#include <controller/Controller.hh>
#include <step/entity/buff/EntityBuffStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/command/flying/FlyingCommandSpawnStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <step/entity/EntityHitPointChangeStep.hh>
#include <state/State.hh>

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
