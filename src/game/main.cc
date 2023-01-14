#include <iostream>
#include <fstream>

#include "command/EntityMoveCommand.hh"
#include "command/EntityWaitCommand.hh"
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
		new octopus::EntitySpawnStep(octopus::Entity { { 6.5, 2.6 }, 1., false})
	};


	spawners_l.push_back(new octopus::CommandSpawnStep(0,  new octopus::EntityMoveCommand(0, 0, {{2, 2}, {2, 5}, {10, 5}}), false));
	for(unsigned long i = 1 ; i < 9 ; ++ i)
	{
		spawners_l.push_back(new octopus::CommandSpawnStep(i,  new octopus::EntityWaitCommand(i, i), false));
	}
	octopus::Controller controller_l(spawners_l, 1.);

	size_t i = 1;
	for( ; i < 60 ; ++ i)
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
