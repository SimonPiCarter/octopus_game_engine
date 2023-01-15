#include "Metrics.hh"

#include "state/State.hh"

namespace octopus
{

void updateStateMetrics(Metrics &metrics_p, State const &state_p)
{
	metrics_p._nbEntities = state_p.getEntities().size();
	metrics_p._nbCommandables = state_p.getCommandables().size();
}

}

std::ostream &operator<<(std::ostream &os_p, octopus::Metrics const &metrics_p)
{
	os_p<<"Metrics[";
	os_p<<"nbStepsApplied = "<<metrics_p._nbStepsApplied;
	os_p<<", nbStepsCompiled = "<<metrics_p._nbStepsCompiled;
	os_p<<", timeApplyingSteps = "<<(metrics_p._timeApplyingSteps / 1e9);
	os_p<<", timeCompilingSteps = "<<(metrics_p._timeCompilingSteps / 1e9);
	os_p<<", nbEntities = "<<metrics_p._nbEntities;
	os_p<<", nbCommandables = "<<metrics_p._nbCommandables;
	os_p<<"]";
	return os_p;
}
