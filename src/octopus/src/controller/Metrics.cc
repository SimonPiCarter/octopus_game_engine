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
	os_p<<", timeApplyingSteps (ns) = "<<metrics_p._timeApplyingSteps;
	os_p<<", maxTimeCompilingSteps (ns) = "<<metrics_p._maxTimeCompilingSteps;
	os_p<<", timeCompilingSteps (ns) = "<<metrics_p._timeCompilingSteps;
	os_p<<", spikeCompilingSteps = "<<metrics_p._spikeCompilingSteps;
	os_p<<", nbEntities = "<<metrics_p._nbEntities;
	os_p<<", nbCommandables = "<<metrics_p._nbCommandables;
	os_p<<"]";
	return os_p;
}

void streamMetrics(std::ostream &os_p, octopus::Metrics const &metrics_p)
{
	// time in micro seconds
	double timeApplyingPerSteps_l = metrics_p._timeApplyingSteps/metrics_p._nbStepsApplied / 1e6;
	double timeCompilingPerSteps_l = metrics_p._timeCompilingSteps/metrics_p._nbStepsCompiled / 1e6;
	double maxCompilingStep_l = metrics_p._maxTimeCompilingSteps / 1e6;

	os_p<<"Metrics : "<<std::endl;
	os_p<<"\t- number of entities in each state : "<<metrics_p._nbEntities<<std::endl;
	os_p<<"\t- number of commandables in each state : "<<metrics_p._nbCommandables<<std::endl;
	os_p<<"\t- number of steps applied : "<<metrics_p._nbStepsApplied<<std::endl;
	os_p<<"\t- number of steps compiled : "<<metrics_p._nbStepsCompiled<<std::endl;
	os_p<<"\t- average time to apply a step : "<<timeApplyingPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t- average time to compile a step : "<<timeCompilingPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t- maxmimum time to compile a step : "<<maxCompilingStep_l <<"ms"<<std::endl;
	os_p<<"\t- number of spike compilation : "<<metrics_p._spikeCompilingSteps<<std::endl;
}
