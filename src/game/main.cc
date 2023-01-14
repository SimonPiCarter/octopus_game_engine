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

	octopus::Controller controller_l(spawners_l, 1.);

	controller_l.commitCommand( new octopus::EntityMoveCommand(0, 0, {{2, 2}, {2, 5}, {10, 5}}));
	for(unsigned long i = 1 ; i < spawners_l.size() ; ++ i)
	{
		controller_l.commitCommand( new octopus::EntityWaitCommand(i, i) );
	}

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
