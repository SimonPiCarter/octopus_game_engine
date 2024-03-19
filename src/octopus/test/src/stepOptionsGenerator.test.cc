#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

///
/// This test suite aims at checking option step generator
/// add and pop steps
///

using namespace octopus;


/// @brief this simple class create 3 options that add 10, 20 or 30 food resources to the player
class TestGenerator : public StepOptionsGenerator
{
public:
	TestGenerator() : StepOptionsGenerator("") {}
    virtual StepOptionsGenerator* newCopy() const override
	{
		return new TestGenerator();
	}

	void genOptions(State const &) override {}

    /// @brief get internal steppable for the given option
    /// @param options_p the option index
    /// @return a vector of steppables (owner is this generator)
    virtual std::vector<Steppable *> genSteppables(State const &state_p, unsigned long options_p) const override
	{
		std::vector<Steppable *> steps_l;

		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = -10.*options_p;

		steps_l.push_back(new PlayerSpendResourceStep(0, map_l));

		return steps_l;
	}

    /// @brief Return the number of available options for this generator
    virtual unsigned long getNumOptions() const override
	{
		return 1000;
	}
};

TEST(stepOptionsGenerator, simple)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	TestGenerator generator_l;
	PlayerAddOptionStep stepAdd_l(0, "test", &generator_l);
	PlayerPopOptionStep stepPop_l(0, "test", 2);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.apply(state_l);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepPop_l.apply(state_l);

	EXPECT_NEAR(20., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_add)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	TestGenerator generator_l;
	PlayerAddOptionStep stepAdd_l(0, "test", &generator_l);
	PlayerPopOptionStep stepPop_l(0, "test", 2);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.apply(state_l);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.revert(state_l, nullptr);
	stepAdd_l.apply(state_l);

	stepPop_l.apply(state_l);

	EXPECT_NEAR(20., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_pop)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	TestGenerator generator_l;
	PlayerAddOptionStep stepAdd_l(0, "test", &generator_l);
	PlayerPopOptionStep stepPop_l(0, "test", 2);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.apply(state_l);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	SteppableData const * data_l = stepPop_l.newData(state_l);
	stepPop_l.apply(state_l);

	EXPECT_NEAR(20., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepPop_l.revert(state_l, data_l);
	delete data_l;

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepPop_l.apply(state_l);

	EXPECT_NEAR(20., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

}

TEST(stepOptionsGenerator, simple_revert_add_then_stop)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	TestGenerator generator_l;
	PlayerAddOptionStep stepAdd_l(0, "test", &generator_l);
	PlayerPopOptionStep stepPop_l(0, "test", 2);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.apply(state_l);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.revert(state_l, nullptr);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_pop_then_stop)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	TestGenerator generator_l;
	PlayerAddOptionStep stepAdd_l(0, "test", &generator_l);
	PlayerPopOptionStep stepPop_l(0, "test", 2);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepAdd_l.apply(state_l);

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	SteppableData const * data_l = stepPop_l.newData(state_l);
	stepPop_l.apply(state_l);

	EXPECT_NEAR(20., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);

	stepPop_l.revert(state_l, data_l);
	delete data_l;

	EXPECT_NEAR(0., to_double(getResource(*state_l.getPlayer(0), "bloc")), 1e-3);
}
