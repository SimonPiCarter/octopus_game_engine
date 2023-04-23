
#include "BuildingRallyPointCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/building/BuildingRemoveRallyPointStep.hh"
#include "step/building/BuildingSetRallyPointStep.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{

/// @brief Rally point command constructor to set the rallypoint
BuildingRallyPointCommand::BuildingRallyPointCommand(Handle const &handle_p, Vector const &rallyPoint_p, bool rallyPointEntityActive_p, Handle const &rallyPointEntity_p)
	: Command(handle_p)
	, _reset(false)
	, _rallyPoint(rallyPoint_p)
	, _rallyPointEntityActive(rallyPointEntityActive_p)
	, _rallyPointEntity(rallyPointEntity_p)
{}

/// @brief Rally point command constructor to reset the rallypoint
BuildingRallyPointCommand::BuildingRallyPointCommand(Handle const &handle_p)
	: Command(handle_p)
	, _reset(true)
	, _rallyPoint(0,0)
	, _rallyPointEntityActive(false)
	, _rallyPointEntity(0)
{}

void BuildingRallyPointCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingRallyPointCommand:: register Command "<<_handleCommand <<std::endl;

	if(_reset)
	{
		step_p.addSteppable(new BuildingRemoveRallyPointStep(_handleCommand));
	}
	else
	{
		step_p.addSteppable(new BuildingSetRallyPointStep(_handleCommand, _rallyPoint, _rallyPointEntityActive, _rallyPointEntity));
	}
	step_p.addSteppable(new CommandStorageStep(this));
}

} // namespace octopus

