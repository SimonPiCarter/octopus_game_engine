#include <fstream>

#include "cases/Cases.hh"

#include "controller/Controller.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "logger/Logger.hh"
#include "library/Library.hh"

int main()
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = Case2(lib_l);

	octopus::Controller controller_l(spawners_l, 1.);

	bool writeFiles_l = true;

	octopus::Logger::getNormal()<<"Playing"<<std::endl;

	size_t i = 1;
	for( ; i < 600 ; ++ i)
	{
		if(i == 1 && writeFiles_l)
		{
			octopus::State const * state_l = controller_l.queryState();
			std::ofstream file_l("step/step_0.csv");
			streamCsvEntity(file_l, state_l->getEntities());
		}

		controller_l.update(1.);

		while(!controller_l.loop_body()) {}

		octopus::State const * state_l = controller_l.queryState();

		if(writeFiles_l)
		{
			std::ofstream file_l("step/step_"+std::to_string(i)+".csv");
			streamCsvEntity(file_l, state_l->getEntities());
		}
	}

	octopus::Logger::getNormal()<<"Done"<<std::endl;

	octopus::LoggerHandler logger_l = octopus::Logger::getNormal();
	streamMetrics(logger_l, controller_l.getMetrics());

	return 0;
}
