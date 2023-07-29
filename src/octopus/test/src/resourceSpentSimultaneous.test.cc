#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/building/BuildingUnitProductionCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerSpawnStep.hh"

///
/// This test suite aims at checking that production command works properly
/// especially when building two things at the same time
///

using namespace octopus;

///
/// > 2 entities
///  - building : 1, 3 production
///  - building : 1, 5 production
/// here we have enough resources
TEST(resourceSpentSimultaneousTest, simple_ok)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;
	unitModel_l._cost["bloc"] = 50;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._unitModels.push_back(&unitModel_l);

	Building production1_l({1,3}, true, buildingModel_l);
	Building production2_l({1,5}, true, buildingModel_l);

	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -100;

	Controller controller_l({
            new PlayerSpawnStep(0, 0),
			new PlayerSpendResourceStep(0, mapRes_l),
            new BuildingSpawnStep(Handle(0), production1_l, true),
            new BuildingSpawnStep(Handle(1), production2_l, true),
        }, 1.);

	controller_l.commitCommand(new BuildingUnitProductionCommand(Handle(1), Handle(1), unitModel_l));
	controller_l.commitCommand(new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 5second (6)
	// at this point production time is over but no spawn yet
	controller_l.update(6.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 1second (7)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(4u, state_l->getEntities().size());
}

///
/// > 2 entities
///  - building : 1, 3 production
///  - building : 1, 5 production
/// here we do not have enough resources -> only one production
TEST(resourceSpentSimultaneousTest, simple_ko)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;
	unitModel_l._cost["bloc"] = 50;

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._unitModels.push_back(&unitModel_l);

	Building production1_l({1,3}, true, buildingModel_l);
	Building production2_l({1,5}, true, buildingModel_l);

	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -50;

	Controller controller_l({
            new PlayerSpawnStep(0, 0),
			new PlayerSpendResourceStep(0, mapRes_l),
            new BuildingSpawnStep(Handle(0), production1_l, true),
            new BuildingSpawnStep(Handle(1), production2_l, true)
        }, 1.);

	controller_l.commitCommand(new BuildingUnitProductionCommand(Handle(1), Handle(1), unitModel_l));
	controller_l.commitCommand(new BuildingUnitProductionCommand(Handle(0), Handle(0), unitModel_l));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 5second (6)
	// at this point production time is over but no spawn yet
	controller_l.update(6.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(2u, state_l->getEntities().size());

	// update time to 1second (7)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(3u, state_l->getEntities().size());
}
