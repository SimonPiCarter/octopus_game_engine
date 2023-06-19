#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/entity/EntityBuildingCommand.hh"
#include "command/building/BuildingBlueprintCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

///
/// This test suite aims at checking that building command works properly
///

using namespace octopus;

///
/// > 2 entities
///  - unit : 5 3
///
TEST(tickingRegenTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10 };
	unitModel_l._isUnit = true;
	unitModel_l._hpRegeneration = 1;
	Unit unit_l({ 5, 3 }, false, unitModel_l);
	unit_l._hp = 5;

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(0, unit_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(0)->_hp), 1e-5);

	// update time to 100 seconds (100)
	controller_l.update(100.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6., to_double(state_l->getEntity(0)->_hp), 1e-5);
}
