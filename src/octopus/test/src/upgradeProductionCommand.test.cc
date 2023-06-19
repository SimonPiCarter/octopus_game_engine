#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/building/BuildingUpgradeProductionCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/player/Player.hh"
#include "state/player/upgrade/Upgrade.hh"
#include "state/player/upgrade/StepUpgradeTyppedBuffGenerator.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

///
/// This test suite aims at checking that production command works properly for upgrade
///

using namespace octopus;



///
/// > 1 entity
///  - building : 1 upgrade
///
TEST(upgradeProductionCommandTest, simple)
{
	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._id = "prod";
	buildingModel_l._isBuilding = true;

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(0, production_l, true);

	TimedBuff buff_l;
	buff_l._type = TyppedBuff::Type::Production;
	buff_l._coef = 0.1;
	Upgrade up_l("upgrade", new StepUpgradeTyppedBuffGenerator(buff_l, {"prod"}));
	up_l._productionTime = 5;

	// up production
	BuildingUpgradeProductionCommand * command_l = new BuildingUpgradeProductionCommand(0, 0, up_l);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, commandSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 5second (5)
	// at this point production time is over but no spawn yet
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 2second (7)
	// one step to commit the buff all entities
	// one step to apply it
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	ASSERT_EQ(1u, state_l->getEntities().size());
	Entity const &ent_l  = *state_l->getEntity(0);
	EXPECT_NEAR(0.1, ent_l._buffProduction._coef.to_double(), 1e-5);
}
