
#include "library/wave_level/WaveLevel.hh"

// octopus
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"
#include "state/State.hh"


int main()
{
	unsigned long gridSize_l = 5;

	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevel(lib_l);

	octopus::Controller controller_l(spawners_l, 0.01, {}, gridSize_l);

	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
    bool over_l = false;

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

	octopus::LoggerHandler logger_l = octopus::Logger::getNormal();
	streamMetrics(logger_l, controller_l.getMetrics());

    return 0;
}