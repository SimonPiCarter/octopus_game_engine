#include <iostream>

#include "command/EntityMoveCommand.hh"
#include "command/EntityWaitCommand.hh"
#include "command/EntityAttackCommand.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

std::list<octopus::Steppable *> Case1()
{
	octopus::EntityModel targetModel_l { false, 5., 0.25, 250. };
	octopus::EntityModel buildingModel_l { true, 2., 0.25, 10. };
	octopus::EntityModel unitModel_l { false, 1., 0.25, 10. };

	octopus::Entity target_l { { 10., 15. }, true, targetModel_l};

	octopus::Entity building_l { {15, 5}, true, buildingModel_l };

	std::list<octopus::Steppable *> spawners_l =
	{
		new octopus::PlayerSpawnStep(0, 0),
		new octopus::EntitySpawnStep(octopus::Entity { { 23.6, 3. }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 25.1, 3.1 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 26.5, 3. }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 23.5, 3.6 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 25., 3.4 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 26.4, 3.5 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 23.2, 2.4 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 25.1, 2.5 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(octopus::Entity { { 26.5, 2.6 }, false, unitModel_l}),
		new octopus::EntitySpawnStep(target_l),
		new octopus::EntitySpawnStep(building_l)
	};

	/// add a ton of entities
	for(unsigned long i = 0 ; i < 1000 ; ++ i)
	{
		spawners_l.push_back(new octopus::EntitySpawnStep(octopus::Entity { { 6.5+2*i%100, 2.6+2*(i/100) }, false, unitModel_l}));
		octopus::EntityMoveCommand * cmd_l = new octopus::EntityMoveCommand(11+i, 11+i, {50, 50}, 0, {{50, 50}});
		spawners_l.push_back(new octopus::CommandSpawnStep(cmd_l));
	}

	for(unsigned long i = 1 ; i < 9 ; ++ i)
	{
		spawners_l.push_back(new octopus::CommandSpawnStep(new octopus::EntityAttackCommand(i, i, 9)));
	}
	{
		spawners_l.push_back(new octopus::CommandSpawnStep(new octopus::EntityMoveCommand(0, 0, {10, 5}, 0, {{2, 2}, {2, 5}, {10, 5}})));
		octopus::EntityAttackCommand *cmd_l = new octopus::EntityAttackCommand(0, 0, 9);
		cmd_l->setQueued(true);
		spawners_l.push_back(new octopus::CommandSpawnStep(cmd_l));
	}
	for(unsigned long i = 0 ; i < 9 ; ++ i)
	{
		octopus::EntityMoveCommand * cmd_l = new octopus::EntityMoveCommand(i, i,  {0, 0}, 0, {{0, 0}});
		cmd_l->setQueued(true);
		spawners_l.push_back(new octopus::CommandSpawnStep(cmd_l));
	}

	return spawners_l;
}
