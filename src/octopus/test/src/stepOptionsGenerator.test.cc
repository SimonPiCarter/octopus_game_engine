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
	~TestGenerator()
	{
		for(auto step_l : _steps)
		{
			delete step_l;
		}
	}

    virtual StepOptionsGenerator* newCopy() const override
	{
		return new TestGenerator();
	}

    /// @brief get internal steppable for the given option
    /// @param options_p the option index
    /// @return a vector of steppables (owner is this generator)
    virtual std::vector<Steppable *> const & getSteppables(unsigned long options_p, unsigned long player_p) override
	{
		// lazy construction
		if(_steps.empty())
		{
			std::map<ResourceType, double> map_l;
			map_l[ResourceType::Food] = -10.*options_p;

			_steps.push_back(new PlayerSpendResourceStep(player_p, map_l));
		}
		return _steps;
	}

    /// @brief Return the number of available options for this generator
    virtual unsigned long getNumOptions() const override
	{
		return 1000;
	}

	std::vector<Steppable *> _steps;
};

TEST(stepOptionsGenerator, simple)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	PlayerAddOptionStep stepAdd_l(0, "test", new TestGenerator());
	PlayerPopOptionStep stepPop_l(0, "test", 2);
	PlayerPopOptionStepData data_l;

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.apply(state_l, nullptr);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepPop_l.apply(state_l, &data_l);

	EXPECT_NEAR(20., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_add)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	PlayerAddOptionStep stepAdd_l(0, "test", new TestGenerator());
	PlayerPopOptionStep stepPop_l(0, "test", 2);
	PlayerPopOptionStepData data_l;

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.apply(state_l, nullptr);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.revert(state_l, nullptr);
	stepAdd_l.apply(state_l, nullptr);

	stepPop_l.apply(state_l, &data_l);

	EXPECT_NEAR(20., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_pop)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	PlayerAddOptionStep stepAdd_l(0, "test", new TestGenerator());
	PlayerPopOptionStep stepPop_l(0, "test", 2);
	PlayerPopOptionStepData data_l;

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.apply(state_l, nullptr);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepPop_l.apply(state_l, &data_l);

	EXPECT_NEAR(20., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepPop_l.revert(state_l, &data_l);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepPop_l.apply(state_l, &data_l);

	EXPECT_NEAR(20., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_add_then_stop)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	PlayerAddOptionStep stepAdd_l(0, "test", new TestGenerator());
	PlayerPopOptionStep stepPop_l(0, "test", 2);
	PlayerPopOptionStepData data_l;

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.apply(state_l, nullptr);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.revert(state_l, nullptr);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);
}

TEST(stepOptionsGenerator, simple_revert_pop_then_stop)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());
	state_l.getPlayers().back()->_id = 0;

	PlayerAddOptionStep stepAdd_l(0, "test", new TestGenerator());
	PlayerPopOptionStep stepPop_l(0, "test", 2);
	PlayerPopOptionStepData data_l;

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepAdd_l.apply(state_l, nullptr);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepPop_l.apply(state_l, &data_l);

	EXPECT_NEAR(20., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);

	stepPop_l.revert(state_l, &data_l);

	EXPECT_NEAR(0., getResource(*state_l.getPlayer(0), ResourceType::Food), 1e-3);
}
