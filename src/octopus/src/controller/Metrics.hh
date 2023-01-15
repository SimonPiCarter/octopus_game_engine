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
	size_t _nbStepsApplied {0};
	size_t _nbStepsCompiled {0};
	/// @brief time in nano seconds
	double _timeApplyingSteps {0.};
	/// @brief time in nano seconds
	double _timeCompilingSteps {0.};
	size_t _nbEntities {0};
	size_t _nbCommandables {0};
};

/// @brief update nb entities and commandables metrics
void updateStateMetrics(Metrics &metrics_p, State const &state_p);

} // octopus

/// @brief raw display of Metrics
std::ostream &operator<<(std::ostream &os_p, octopus::Metrics const &metrics_p);

/// @brief prettified display of Metrics
void streamMetrics(std::ostream &os_p, octopus::Metrics const &metrics_p);

#endif
