#include <gtest/gtest.h>

#include "state/State.hh"
#include "step/state/StateWinStep.hh"

using namespace octopus;

TEST(StateWinStepTest, simple)
{
    State state_l;

    StateWinStep winStep_l(false, false, 0, 1);

    EXPECT_FALSE(state_l.isOver());
    EXPECT_FALSE(state_l.hasWinningTeam());
    EXPECT_EQ(0, state_l.getWinningTeam());

    winStep_l.apply(state_l, nullptr);

    EXPECT_TRUE(state_l.isOver());
    EXPECT_TRUE(state_l.hasWinningTeam());
    EXPECT_EQ(1, state_l.getWinningTeam());

    winStep_l.revert(state_l, nullptr);

    EXPECT_FALSE(state_l.isOver());
    EXPECT_FALSE(state_l.hasWinningTeam());
    EXPECT_EQ(0, state_l.getWinningTeam());
}
