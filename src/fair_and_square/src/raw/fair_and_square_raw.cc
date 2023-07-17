
#include "library/levels/MazeLevel.hh"
#include "library/levels/ArenaLevel.hh"
#include "library/levels/WaveLevel.hh"

// octopus
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"
#include "state/State.hh"


int main()
{
	octopus::RandomGenerator rand_l(42);
	octopus::Library lib_l;
	// std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, rand_l, 15, 0.1*60*100, 0, 250);
	// std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l, rand_l, 250);

    std::vector<ArenaInfo> you_l;
	you_l.push_back({500, "square"});
	you_l.push_back({500, "circle"});
	you_l.push_back({500, "triangle"});
    std::vector<ArenaInfo> them_l;
	them_l.push_back({500, "square"});
	them_l.push_back({500, "circle"});
	them_l.push_back({500, "triangle"});
	std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(lib_l, you_l, them_l);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(lib_l);

	octopus::Controller controller_l(spawners_l, 0.01, commands_l, 5, 50);
	controller_l.enableORCA();

	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
    bool over_l = false;

	octopus::Logger::getNormal()<<"Playing"<<std::endl;

	// print progress variables
	size_t const total_l = 6000;
	size_t lastPrint_l = 0;
	size_t const stepPrint_l = 100;

	while(!over_l && controller_l.getMetrics()._nbStepsCompiled < total_l)
	{
        // update controller
        controller_l.update(elapsed_l);

		controller_l.loop_body();

		octopus::State const * state_l = controller_l.queryState();

        over_l = state_l->isOver();

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;
		if(controller_l.getMetrics()._nbStepsCompiled%stepPrint_l == 0 && lastPrint_l != controller_l.getMetrics()._nbStepsCompiled)
		{
			lastPrint_l = controller_l.getMetrics()._nbStepsCompiled;
			octopus::Logger::getNormal()<<lastPrint_l/stepPrint_l<<"/"<<total_l/stepPrint_l<<std::endl;
		}
	}

	octopus::Logger::getNormal()<<"Done"<<std::endl;

	octopus::LoggerHandler logger_l = octopus::Logger::getNormal();
	streamMetrics(logger_l, controller_l.getMetrics());

    return 0;
}
