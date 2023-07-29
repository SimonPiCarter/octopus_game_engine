#include <iostream>

#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "library/Library.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

std::list<Steppable *> Case1(Library &lib_p)
{
	EntityModel targetModel_l { false, 5., 0.25, 250. };
	EntityModel buildingModel_l { true, 2., 0.25, 10. };
	EntityModel unitModel_l { false, 1., 0.25, 10. };

	lib_p.registerEntityModel("target", targetModel_l);
	lib_p.registerEntityModel("building", buildingModel_l);
	lib_p.registerEntityModel("unit", unitModel_l);

	Entity target_l { { 10., 15. }, true, lib_p.getEntityModel("target")};

	Entity building_l { {15, 5}, true, lib_p.getEntityModel("building") };

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(Handle(0), Entity { { 23.6, 3. }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(1), Entity { { 25.1, 3.1 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(2), Entity { { 26.5, 3. }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(3), Entity { { 23.5, 3.6 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(4), Entity { { 25., 3.4 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(5), Entity { { 26.4, 3.5 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(6), Entity { { 23.2, 2.4 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(7), Entity { { 25.1, 2.5 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(8), Entity { { 26.5, 2.6 }, false, lib_p.getEntityModel("unit")}),
		new EntitySpawnStep(Handle(9), target_l),
		new EntitySpawnStep(Handle(10) building_l)
	};

	/// add a ton of entities
	for(unsigned long i = 0 ; i < 1000 ; ++ i)
	{
		spawners_l.push_back(new EntitySpawnStep(11+i, Entity { { 6.5+2*i%100, 2.6+2*(i/100) }, false, unitModel_l}));
		EntityMoveCommand * cmd_l = new EntityMoveCommand(11+i, 11+i, {50, 50}, 0, {{50, 50}});
		spawners_l.push_back(new CommandSpawnStep(cmd_l));
	}

	for(unsigned long i = 1 ; i < 9 ; ++ i)
	{
		spawners_l.push_back(new CommandSpawnStep(new EntityAttackCommand(i, i, 9, true)));
	}
	{
		spawners_l.push_back(new CommandSpawnStep(new EntityMoveCommand(0, 0, {10, 5}, 0, {{2, 2}, {2, 5}, {10, 5}})));
		EntityAttackCommand *cmd_l = new EntityAttackCommand(0, 0, 9, true);
		cmd_l->setQueued(true);
		spawners_l.push_back(new CommandSpawnStep(cmd_l));
	}
	for(unsigned long i = 0 ; i < 9 ; ++ i)
	{
		EntityMoveCommand * cmd_l = new EntityMoveCommand(i, i,  {0, 0}, 0, {{0, 0}});
		cmd_l->setQueued(true);
		spawners_l.push_back(new CommandSpawnStep(cmd_l));
	}

	return spawners_l;
}
