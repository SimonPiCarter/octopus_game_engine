#include <gtest/gtest.h>

#include "state/State.hh"
#include "step/state/StateDrawStep.hh"

using namespace octopus;

TEST(stateDrawStepTest, simple)
{
    State state_l;

    StateDrawStep drawStep_l(false, false);

    EXPECT_FALSE(state_l.isOver());
    EXPECT_FALSE(state_l.hasWinningTeam());

    drawStep_l.apply(state_l);

    EXPECT_TRUE(state_l.isOver());
    EXPECT_FALSE(state_l.hasWinningTeam());

    drawStep_l.revert(state_l);

    EXPECT_FALSE(state_l.isOver());
    EXPECT_FALSE(state_l.hasWinningTeam());
}