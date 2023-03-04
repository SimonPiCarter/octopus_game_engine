
#include "library/levels/WaveLevel.hh"

// octopus
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"
#include "state/State.hh"


int main()
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, 15, 0.1*60*100);
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l);

	octopus::Controller controller_l(spawners_l, 0.01, commands_l, 5, 100);

	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
    bool over_l = false;

	octopus::Logger::getNormal()<<"Playing"<<std::endl;

	while(!over_l)
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
	}

	octopus::Logger::getNormal()<<"Done"<<std::endl;

	octopus::LoggerHandler logger_l = octopus::Logger::getNormal();
	streamMetrics(logger_l, controller_l.getMetrics());

    return 0;
}
