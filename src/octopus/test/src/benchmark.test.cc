#include <gtest/gtest.h>

#include <command/entity/EntityAttackCommand.hh>
#include <controller/Controller.hh>
#include <step/Step.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>

#include <chrono>
using namespace std::chrono_literals;

///
/// This test suite aims at checking that EntityAttackCommand works properly
/// - Move an entity to the target
/// - Attack the target then terminate
///

using namespace octopus;

TEST(DISABLED_benchmarkTest, simple)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	octopus::State state_l;

	size_t nb_l = 5000;

	octopus::Step step_l(nullptr);
	octopus::StepData stepData_l;
	for(size_t i = 0 ; i < nb_l ; ++i)
	{
		state_l.addEntity(new octopus::Entity {{ 3, 3. }, false, unitModel_l});
		step_l.addEntityMoveStep(new EntityMoveStep(i, {1, 0}));
	}

    auto start = std::chrono::steady_clock::now();

	apply(step_l, state_l, stepData_l);

    auto end = std::chrono::steady_clock::now();
    std::cout<<(end - start) / 1us <<"us"<< std::endl;
	start = end;

	revert(step_l, state_l, stepData_l);

    end = std::chrono::steady_clock::now();
    std::cout<<(end - start) / 1us <<"us"<< std::endl;
}
