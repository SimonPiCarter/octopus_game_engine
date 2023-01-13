#include <iostream>
#include <fstream>

#include "command/EntityMoveCommand.hh"
#include "command/EntityWaitCommand.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/ConflictPositionSolver.hh"
#include "step/Step.hh"

#include "graph/Grid.hh"

int main()
{
	///
	/// Create a path from a grid
	///
	///
	octopus::Grid grid_l(10, 10, 1., 1.);
	octopus::Graph &graph_l = grid_l.getGraph();

	std::list<octopus::GridNode const *> path_l = graph_l.getPath(grid_l.getNode(0,0), grid_l.getNode(9,9));
	trimPath(path_l);

	for(octopus::GridNode const * node_l : path_l)
	{
		std::cout<<node_l->getPosition()<<std::endl;
	}

	for(size_t i = 1; i < 8 ; ++ i)
	{
		for(size_t j = 1; j < 8 ; ++ j)
		{
			grid_l.getNode(i,j)->setFree(false);
		}
	}
	graph_l.update();

	path_l = graph_l.getPath(grid_l.getNode(0,0), grid_l.getNode(9,9));
	trimPath(path_l);

	for(octopus::GridNode const * node_l : path_l)
	{
		std::cout<<node_l->getPosition()<<std::endl;
	}

	octopus::State state_l;
	///
	/// Create entities and command
	///
	state_l.addEntity(new octopus::Entity { { 3.6, 3. }, 1., false});
	state_l.addEntity(new octopus::Entity { { 5.1, 3.1 }, 1., true});
	state_l.addEntity(new octopus::Entity { { 6.5, 3. }, 1., false});
	state_l.addEntity(new octopus::Entity { { 3.5, 3.6 }, 1., false});
	state_l.addEntity(new octopus::Entity { { 5., 3.4 }, 1., false});
	state_l.addEntity(new octopus::Entity { { 6.4, 3.5 }, 1., false});
	state_l.addEntity(new octopus::Entity { { 3.2, 2.4 }, 1., false});
	state_l.addEntity(new octopus::Entity { { 5.1, 2.5 }, 1., false});
	state_l.addEntity(new octopus::Entity { { 6.5, 2.6 }, 1., false});

	std::vector<octopus::Command *> vecCommands_l;
	vecCommands_l.push_back(new octopus::EntityMoveCommand(0, 0, octopus::toWaypoints(path_l)));

	for(size_t i = 1 ; i < state_l.getEntities().size() ; ++i)
	{
		vecCommands_l.push_back(new octopus::EntityWaitCommand(i, i));
	}

	std::list<octopus::Step *> allSteps_l;

	size_t i = 1;
	for( ; i < 60 ; ++ i)
	{
		allSteps_l.push_back(new octopus::Step());

		octopus::Step &step_l = *allSteps_l.back();

		for(octopus::Command * cmd_l : vecCommands_l)
		{
			cmd_l->applyCommand(step_l, state_l);
		}

		if(i == 1)
		{
			std::ofstream file_l("step/step_0.csv");
			streamCsvEntity(file_l, state_l.getEntities());
		}

		octopus::updateStepFromConflictPosition(step_l, state_l);
		octopus::compact(step_l);
		octopus::apply(step_l, state_l);

		std::ofstream file_l("step/step_"+std::to_string(i)+".csv");
		streamCsvEntity(file_l, state_l.getEntities());
	}

	// Reverse all steps
	for(auto it_l = allSteps_l.rbegin(); it_l != allSteps_l.rend(); ++it_l)
	{
		octopus::revert(**it_l, state_l);

		std::ofstream file_l("step/step_"+std::to_string(i++)+".csv");
		streamCsvEntity(file_l, state_l.getEntities());
	}

	for(octopus::Command * cmd_l : vecCommands_l)
	{
		delete cmd_l;
	}

	std::cout<<"Done"<<std::endl;
	return 0;
}
