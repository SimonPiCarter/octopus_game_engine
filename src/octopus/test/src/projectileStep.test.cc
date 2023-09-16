#include <gtest/gtest.h>

#include "step/TickingStep.hh"
#include "step/projectile/ProjectileSpawnStep.hh"
#include "step/Step.hh"

///
/// This test suite aims at checking that PlayerBuffAllStep works properly
/// - check that only given player is affected
/// - check that only model is affected
/// - check that revert is correctly applied
/// - check that it applies to spawned unit after buff has been applied
///

using namespace octopus;

TEST(ProjectileStepTest, simple)
{
	State state_l;
	ProjectileSpawnStep spawnStep_l;

	Projectile proj_l;
	proj_l._posTarget = Vector(5, 0);
	proj_l._speed = 1;

	spawnStep_l.addProjectile(state_l.getProjectileContainer(), std::move(proj_l));

	Projectile proj2_l;
	proj2_l._posTarget = Vector(0, 5);
	proj2_l._speed = 2.5;

	spawnStep_l.addProjectile(state_l.getProjectileContainer(), std::move(proj2_l));

	ASSERT_EQ(2u, spawnStep_l.getToBeSpawned().size());
	EXPECT_EQ(0u, spawnStep_l.getToBeSpawned().at(0)._index);
	EXPECT_EQ(1u, spawnStep_l.getToBeSpawned().at(1)._index);

	spawnStep_l.apply(state_l);

	ASSERT_EQ(2u, state_l.getProjectileContainer().getProjectiles().size());
	EXPECT_EQ(0u, state_l.getProjectileContainer().getProjectiles().at(0)._index);
	EXPECT_EQ(1u, state_l.getProjectileContainer().getProjectiles().at(1)._index);

	Step step_l(nullptr);
	for(Projectile const &curProj_l : state_l.getProjectileContainer().getProjectiles())
	{
		tickProjectile(step_l, curProj_l, state_l);
	}

	ProjectileMoveStep const& moveStep_l = step_l.getProjectileMoveStep();

	StepData data_l;
	apply(step_l, state_l, data_l);

	ASSERT_EQ(2u, state_l.getProjectileContainer().getProjectiles().size());

	EXPECT_EQ(Fixed::OneAsLong(), state_l.getProjectileContainer().getProjectiles().at(0)._pos.x.data());
	EXPECT_EQ(0, state_l.getProjectileContainer().getProjectiles().at(0)._pos.y.data());

	EXPECT_EQ(0, state_l.getProjectileContainer().getProjectiles().at(1)._pos.x.data());
	EXPECT_EQ(Fixed::OneAsLong()*2.5, state_l.getProjectileContainer().getProjectiles().at(1)._pos.y.data());
}
