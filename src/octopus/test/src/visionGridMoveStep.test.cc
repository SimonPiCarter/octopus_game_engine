#include <gtest/gtest.h>

#include <functional>

#include "logger/Logger.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/vision/VisionHandler.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/vision/VisionChangeStep.hh"

using namespace octopus;

void dumpVision(VisionHandler const &handler_p)
{
	for(unsigned long i = 0 ; i < 10; ++i)
	{
		for(unsigned long j = 0 ; j < 10; ++j)
		{
			std::cout<<(handler_p.isVisible(0, j, i)?"1":"0")<<", ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

/// this test aims at checking that vision
/// behaves correctly when moving
TEST(VisionGridMoveStep, move_simple)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._lineOfSight = 2;

	State state_l(0, 1, 10);
	VisionHandler & handler_l = state_l.getVisionHandler();

	EXPECT_FALSE(handler_l.isVisible(0, 3, 3));

	Step initial_l(nullptr);
	StepData initialData_l;

	initial_l.addSteppable(new PlayerSpawnStep(0, 0));
	initial_l.addSteppable(new EntitySpawnStep(0, Entity { { 3., 3. }, false, unitModel_l}));

	apply(initial_l, state_l, initialData_l);

	EXPECT_TRUE(handler_l.isVisible(0, 3, 3));


	std::vector<std::vector<int> > vision_l = {
	//   0 1 2 3 4 5 6 7 8 9
		{0,0,0,0,0,0,0,0,0,0}, // 0
		{0,0,0,0,0,0,0,0,0,0}, // 1
		{0,0,1,1,1,0,0,0,0,0}, // 2
		{0,0,1,1,1,0,0,0,0,0}, // 3
		{0,0,1,1,1,0,0,0,0,0}, // 4
		{0,0,0,0,0,0,0,0,0,0}, // 5
		{0,0,0,0,0,0,0,0,0,0}, // 6
		{0,0,0,0,0,0,0,0,0,0}, // 7
		{0,0,0,0,0,0,0,0,0,0}, // 8
		{0,0,0,0,0,0,0,0,0,0}, // 9
	};

	for(unsigned long x = 0 ; x < 10; ++x)
	{
		for(unsigned long y = 0 ; y < 10; ++y)
		{
			EXPECT_EQ(vision_l[y][x] == 1, handler_l.isVisible(0, x, y)) << "expected vision on "<<x << ", "<<y<< " to be equal to "<<vision_l[y][x];
		}
	}

	///
	/// Second move
	///
	Step second_l(&initial_l);
	StepData secondData_l;
	second_l.addEntityMoveStep(new EntityMoveStep(0, Vector {0.5, 0.}));

	std::list<VisionChangeStep *> list_l = newVisionChangeStep(state_l, second_l, state_l.getWorldSize(), handler_l.getPatternHandler());
	std::for_each(list_l.begin(), list_l.end(), std::bind(&Step::addSteppable, &second_l, std::placeholders::_1));

	apply(second_l, state_l, secondData_l);

	vision_l = {
	//   0 1 2 3 4 5 6 7 8 9
		{0,0,0,0,0,0,0,0,0,0}, // 0
		{0,0,0,0,0,0,0,0,0,0}, // 1
		{0,0,1,1,1,0,0,0,0,0}, // 2
		{0,0,1,1,1,0,0,0,0,0}, // 3
		{0,0,1,1,1,0,0,0,0,0}, // 4
		{0,0,0,0,0,0,0,0,0,0}, // 5
		{0,0,0,0,0,0,0,0,0,0}, // 6
		{0,0,0,0,0,0,0,0,0,0}, // 7
		{0,0,0,0,0,0,0,0,0,0}, // 8
		{0,0,0,0,0,0,0,0,0,0}, // 9
	};

	for(unsigned long x = 0 ; x < 10; ++x)
	{
		for(unsigned long y = 0 ; y < 10; ++y)
		{
			EXPECT_EQ(vision_l[y][x] == 1, handler_l.isVisible(0, x, y)) << "expected vision on "<<x << ", "<<y<< " to be equal to "<<vision_l[y][x];
		}
	}


	///
	/// Third move
	///
	Step third_l(&initial_l);
	StepData thirdData_l;
	third_l.addEntityMoveStep(new EntityMoveStep(0, Vector {0.5, 0.}));

	list_l = newVisionChangeStep(state_l, third_l, state_l.getWorldSize(), handler_l.getPatternHandler());
	std::for_each(list_l.begin(), list_l.end(), std::bind(&Step::addSteppable, &third_l, std::placeholders::_1));

	apply(third_l, state_l, thirdData_l);

	vision_l = {
	//   0 1 2 3 4 5 6 7 8 9
		{0,0,0,0,0,0,0,0,0,0}, // 0
		{0,0,0,0,0,0,0,0,0,0}, // 1
		{0,0,0,1,1,1,0,0,0,0}, // 2
		{0,0,0,1,1,1,0,0,0,0}, // 3
		{0,0,0,1,1,1,0,0,0,0}, // 4
		{0,0,0,0,0,0,0,0,0,0}, // 5
		{0,0,0,0,0,0,0,0,0,0}, // 6
		{0,0,0,0,0,0,0,0,0,0}, // 7
		{0,0,0,0,0,0,0,0,0,0}, // 8
		{0,0,0,0,0,0,0,0,0,0}, // 9
	};

	for(unsigned long x = 0 ; x < 10; ++x)
	{
		for(unsigned long y = 0 ; y < 10; ++y)
		{
			EXPECT_EQ(vision_l[y][x] == 1, handler_l.isVisible(0, x, y)) << "expected vision on "<<x << ", "<<y<< " to be equal to "<<vision_l[y][x];
		}
	}

}


/// this test aims at checking that vision
/// behaves correctly when moving two entities
TEST(VisionGridMoveStep, move_two)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._lineOfSight = 2;

	State state_l(0, 1, 10);
	VisionHandler & handler_l = state_l.getVisionHandler();

	EXPECT_FALSE(handler_l.isVisible(0, 3, 3));

	Step initial_l(nullptr);
	StepData initialData_l;

	initial_l.addSteppable(new PlayerSpawnStep(0, 0));
	initial_l.addSteppable(new EntitySpawnStep(0, Entity { { 3., 3. }, false, unitModel_l}));
	initial_l.addSteppable(new EntitySpawnStep(1, Entity { { 4., 3. }, false, unitModel_l}));

	apply(initial_l, state_l, initialData_l);

	EXPECT_TRUE(handler_l.isVisible(0, 3, 3));


	std::vector<std::vector<int> > vision_l = {
	//   0 1 2 3 4 5 6 7 8 9
		{0,0,0,0,0,0,0,0,0,0}, // 0
		{0,0,0,0,0,0,0,0,0,0}, // 1
		{0,0,1,1,1,1,0,0,0,0}, // 2
		{0,0,1,1,1,1,0,0,0,0}, // 3
		{0,0,1,1,1,1,0,0,0,0}, // 4
		{0,0,0,0,0,0,0,0,0,0}, // 5
		{0,0,0,0,0,0,0,0,0,0}, // 6
		{0,0,0,0,0,0,0,0,0,0}, // 7
		{0,0,0,0,0,0,0,0,0,0}, // 8
		{0,0,0,0,0,0,0,0,0,0}, // 9
	};

	for(unsigned long x = 0 ; x < 10; ++x)
	{
		for(unsigned long y = 0 ; y < 10; ++y)
		{
			EXPECT_EQ(vision_l[y][x] == 1, handler_l.isVisible(0, x, y)) << "expected vision on "<<x << ", "<<y<< " to be equal to "<<vision_l[y][x];
		}
	}

	///
	/// Second move
	///
	Step second_l(&initial_l);
	StepData secondData_l;
	second_l.addEntityMoveStep(new EntityMoveStep(0, Vector {0.5, 0.}));
	second_l.addEntityMoveStep(new EntityMoveStep(1, Vector {0.5, 0.}));

	std::list<VisionChangeStep *> list_l = newVisionChangeStep(state_l, second_l, state_l.getWorldSize(), handler_l.getPatternHandler());
	std::for_each(list_l.begin(), list_l.end(), std::bind(&Step::addSteppable, &second_l, std::placeholders::_1));

	apply(second_l, state_l, secondData_l);

	vision_l = {
	//   0 1 2 3 4 5 6 7 8 9
		{0,0,0,0,0,0,0,0,0,0}, // 0
		{0,0,0,0,0,0,0,0,0,0}, // 1
		{0,0,1,1,1,1,0,0,0,0}, // 2
		{0,0,1,1,1,1,0,0,0,0}, // 3
		{0,0,1,1,1,1,0,0,0,0}, // 4
		{0,0,0,0,0,0,0,0,0,0}, // 5
		{0,0,0,0,0,0,0,0,0,0}, // 6
		{0,0,0,0,0,0,0,0,0,0}, // 7
		{0,0,0,0,0,0,0,0,0,0}, // 8
		{0,0,0,0,0,0,0,0,0,0}, // 9
	};

	for(unsigned long x = 0 ; x < 10; ++x)
	{
		for(unsigned long y = 0 ; y < 10; ++y)
		{
			EXPECT_EQ(vision_l[y][x] == 1, handler_l.isVisible(0, x, y)) << "expected vision on "<<x << ", "<<y<< " to be equal to "<<vision_l[y][x];
		}
	}


	///
	/// Third move
	///
	Step third_l(&initial_l);
	StepData thirdData_l;
	third_l.addEntityMoveStep(new EntityMoveStep(0, Vector {0.5, 0.}));
	third_l.addEntityMoveStep(new EntityMoveStep(1, Vector {0.5, 0.}));

	list_l = newVisionChangeStep(state_l, third_l, state_l.getWorldSize(), handler_l.getPatternHandler());
	std::for_each(list_l.begin(), list_l.end(), std::bind(&Step::addSteppable, &third_l, std::placeholders::_1));

	apply(third_l, state_l, thirdData_l);

	vision_l = {
	//   0 1 2 3 4 5 6 7 8 9
		{0,0,0,0,0,0,0,0,0,0}, // 0
		{0,0,0,0,0,0,0,0,0,0}, // 1
		{0,0,0,1,1,1,1,0,0,0}, // 2
		{0,0,0,1,1,1,1,0,0,0}, // 3
		{0,0,0,1,1,1,1,0,0,0}, // 4
		{0,0,0,0,0,0,0,0,0,0}, // 5
		{0,0,0,0,0,0,0,0,0,0}, // 6
		{0,0,0,0,0,0,0,0,0,0}, // 7
		{0,0,0,0,0,0,0,0,0,0}, // 8
		{0,0,0,0,0,0,0,0,0,0}, // 9
	};

	for(unsigned long x = 0 ; x < 10; ++x)
	{
		for(unsigned long y = 0 ; y < 10; ++y)
		{
			EXPECT_EQ(vision_l[y][x] == 1, handler_l.isVisible(0, x, y)) << "expected vision on "<<x << ", "<<y<< " to be equal to "<<vision_l[y][x];
		}
	}

}
