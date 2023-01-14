#include <iostream>
#include <fstream>

#include "command/EntityMoveCommand.hh"
#include "command/EntityWaitCommand.hh"
#include "command/EntityAttackCommand.hh"
#include "graph/Grid.hh"
#include "logger/Logger.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/EntitySpawnStep.hh"
#include "step/command/CommandQueueStep.hh"
#include "controller/Controller.hh"

int main()
{
	octopus::Entity target_l { { -10., 15. }, 5., false};
	target_l._stats._hp = 1000;
	std::list<octopus::Steppable *> spawners_l =
	{
		new octopus::EntitySpawnStep(octopus::Entity { { 3.6, 3. }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 5.1, 3.1 }, 1., true}),
		new octopus::EntitySpawnStep(octopus::Entity { { 6.5, 3. }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 3.5, 3.6 }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 5., 3.4 }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 6.4, 3.5 }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 3.2, 2.4 }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 5.1, 2.5 }, 1., false}),
		new octopus::EntitySpawnStep(octopus::Entity { { 6.5, 2.6 }, 1., false}),
		new octopus::EntitySpawnStep(target_l)
	};

	spawners_l.push_back(new octopus::CommandSpawnStep(new octopus::EntityMoveCommand(0, 0, {{2, 2}, {2, 5}, {10, 5}})));
	for(unsigned long i = 1 ; i < 9 ; ++ i)
	{
		spawners_l.push_back(new octopus::CommandSpawnStep(new octopus::EntityAttackCommand(i, i, 9)));
	}

	octopus::EntityAttackCommand *cmd_l = new octopus::EntityAttackCommand(0, 0, 9);
	cmd_l->setQueued(true);
	spawners_l.push_back(new octopus::CommandSpawnStep(cmd_l));

	octopus::Controller controller_l(spawners_l, 1.);

	size_t i = 1;
	for( ; i < 240 ; ++ i)
	{
		if(i == 1)
		{
			octopus::State const * state_l = controller_l.queryState();
			std::ofstream file_l("step/step_0.csv");
			streamCsvEntity(file_l, state_l->getEntities());
		}

		controller_l.update(1.);

		while(!controller_l.loop_body()) {}

		octopus::State const * state_l = controller_l.queryState();

		std::ofstream file_l("step/step_"+std::to_string(i)+".csv");
		streamCsvEntity(file_l, state_l->getEntities());
	}

	octopus::Logger::getNormal()<<"Done"<<std::endl;
	return 0;
}
