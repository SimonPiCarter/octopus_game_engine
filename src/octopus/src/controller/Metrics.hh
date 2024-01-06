#ifndef __Metrics__
#define __Metrics__

#include <mutex>
#include <list>
#include <vector>

#include "step/Step.hh"

namespace octopus
{

struct Metrics
{
	uint32_t _nbStepsApplied {0};
	uint32_t _nbStepsCompiled {0};
	/// @brief time in nano seconds
	double _timeApplyingSteps {0.};
	/// @brief time in nano seconds
	double _timeCompilingSteps {0.};
	/// @brief time computing projectile
	double _timeTickingProjectiles {0.};
	double _timeRunningCommands {0.};
	double _timeCollisionHandling {0.};
	double _timeCollisionConstraint {0.};
	double _timeTrigger {0.};
	double _timeVisionChange {0.};
	double _timeCompactingSteps {0.};
	double _maxTimeCompilingSteps {0.};
	uint32_t _spikeCompilingSteps {0};
	uint32_t _nbEntities {0};
	uint32_t _nbCommandables {0};
	std::vector<double> _vecTimeCompilingSteps;
};

/// @brief update nb entities and commandables metrics
void updateStateMetrics(Metrics &metrics_p, State const &state_p);

} // octopus

/// @brief raw display of Metrics
std::ostream &operator<<(std::ostream &os_p, octopus::Metrics const &metrics_p);

/// @brief prettified display of Metrics
void streamMetrics(std::ostream &os_p, octopus::Metrics const &metrics_p);

#endif
