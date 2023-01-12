#include <iostream>
#include <fstream>

#include "entity/Entity.hh"
#include "entity/EntityStep.hh"
#include "step/ConflictPositionSolver.hh"
#include "step/Step.hh"

#include "graph/Grid.hh"

int main()
{
	game_engine::Grid grid_l(10, 10, 1., 1.);
	game_engine::Graph &graph_l = grid_l.getGraph();



	std::list<game_engine::GridNode const *> path_l = graph_l.getPath(grid_l.getNode(0,0), grid_l.getNode(9,9));
	trimPath(path_l);

	for(game_engine::GridNode const * node_l : path_l)
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

	for(game_engine::GridNode const * node_l : path_l)
	{
		std::cout<<node_l->getPosition()<<std::endl;
	}

	return 0;

	std::cout<<"Hello"<<std::endl;

	std::vector<game_engine::Entity> vecEntity_l;
	vecEntity_l.emplace_back(game_engine::Entity { { 3.6, 3. }, 1., true});
	vecEntity_l.emplace_back(game_engine::Entity { { 5.1, 3.1 }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 6.5, 3. }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 3.5, 3.6 }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 5., 3.4 }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 6.4, 3.5 }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 3.2, 2.4 }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 5.1, 2.5 }, 1., false});
	vecEntity_l.emplace_back(game_engine::Entity { { 6.5, 2.6 }, 1., false});

	for(size_t i = 1 ; i < 60 ; ++ i)
	{
		std::vector<game_engine::EntityStep> vecEntityStep_l;
		game_engine::Step step_l;
		for(game_engine::Entity &ent_l : vecEntity_l)
		{
			if (vecEntityStep_l.size() == 0)
			{
				vecEntityStep_l.emplace_back(game_engine::createEntityStep(ent_l));
			}
			else if(vecEntityStep_l.size() > 5)
			{
				vecEntityStep_l.emplace_back(game_engine::createEntityStep(ent_l, {-5., 8.}, 0.25));
			}
			else
			{
				vecEntityStep_l.emplace_back(game_engine::createEntityStep(ent_l, {15., -5.}, 0.25));
			}
		}
		for(game_engine::EntityStep &ent_l : vecEntityStep_l)
		{
			step_l.addEntityStep(new game_engine::EntityStep(ent_l));
		}
		if(i == 1)
		{
			std::ofstream file_l("step/step_0.csv");
			streamCsvEntity(file_l, step_l);
		}

		game_engine::updateStepFromConflictPosition(step_l);
		game_engine::apply(step_l);

		std::ofstream file_l("step/step_"+std::to_string(i)+".csv");
		streamCsvEntity(file_l, step_l);
	}

	return 0;
}
