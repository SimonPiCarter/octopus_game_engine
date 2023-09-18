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
	os_p<<", timeCompilingSteps (ns) = "<<metrics_p._timeCompilingSteps;
	os_p<<", timeRunningCommands (ns) = "<<metrics_p._timeRunningCommands;
	os_p<<", timeTickingProjectiles (ns) = "<<metrics_p._timeTickingProjectiles;
	os_p<<", timeCollisionHandling (ns) = "<<metrics_p._timeCollisionHandling;
	os_p<<", timeCollisionConstraint (ns) = "<<metrics_p._timeCollisionConstraint;
	os_p<<", timeTrigger (ns) = "<<metrics_p._timeTrigger;
	os_p<<", timeVisionChange (ns) = "<<metrics_p._timeVisionChange;
	os_p<<", timeCompactingSteps (ns) = "<<metrics_p._timeCompactingSteps;
	os_p<<", maxTimeCompilingSteps (ns) = "<<metrics_p._maxTimeCompilingSteps;
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
	double timeRunningCommandsPerSteps_l = metrics_p._timeRunningCommands/metrics_p._nbStepsCompiled / 1e6;
	double timeTickingProjectilesPerSteps_l = metrics_p._timeTickingProjectiles/metrics_p._nbStepsCompiled / 1e6;
	double timeCollisionHandlingPerSteps_l = metrics_p._timeCollisionHandling/metrics_p._nbStepsCompiled / 1e6;
	double timeCollisionConstraintPerSteps_l = metrics_p._timeCollisionConstraint/metrics_p._nbStepsCompiled / 1e6;
	double timeTriggerPerSteps_l = metrics_p._timeTrigger/metrics_p._nbStepsCompiled / 1e6;
	double timeVisionChangePerSteps_l = metrics_p._timeVisionChange/metrics_p._nbStepsCompiled / 1e6;
	double timeCompactingStepsPerSteps_l = metrics_p._timeCompactingSteps/metrics_p._nbStepsCompiled / 1e6;
	double maxCompilingStep_l = metrics_p._maxTimeCompilingSteps / 1e6;

	os_p<<"Metrics : "<<std::endl;
	os_p<<"\t- number of entities in each state : "<<metrics_p._nbEntities<<std::endl;
	os_p<<"\t- number of commandables in each state : "<<metrics_p._nbCommandables<<std::endl;
	os_p<<"\t- number of steps applied : "<<metrics_p._nbStepsApplied<<std::endl;
	os_p<<"\t- number of steps compiled : "<<metrics_p._nbStepsCompiled<<std::endl;
	os_p<<"\t- average time to apply a step : "<<timeApplyingPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t- average time to compile a step : "<<timeCompilingPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to run command each step : "<<timeRunningCommandsPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to tick projectiles each step : "<<timeTickingProjectilesPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to handle collision each step : "<<timeCollisionHandlingPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to handle constraint position each step : "<<timeCollisionConstraintPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to handle trigger each step : "<<timeTriggerPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to handle vision change each step : "<<timeVisionChangePerSteps_l <<"ms"<<std::endl;
	os_p<<"\t\t- average time to handle compacting each step : "<<timeCompactingStepsPerSteps_l <<"ms"<<std::endl;
	os_p<<"\t- maxmimum time to compile a step : "<<maxCompilingStep_l <<"ms"<<std::endl;
	os_p<<"\t- number of spike compilation : "<<metrics_p._spikeCompilingSteps<<std::endl;
}
