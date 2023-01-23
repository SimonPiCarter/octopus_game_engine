
#include "BuildingCommand.hh"

#include <cmath>

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "step/Step.hh"
#include "step/building/BuildingStep.hh"
#include "utils/Box.hh"

namespace octopus
{

BuildingCommand::BuildingCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _target(target_p)
	, _subMoveCommand(commandHandle_p, source_p, finalPoint_p, gridStatus_p, waypoints_p)
{}

bool isInRange(State const &state_p, Entity const * ent_p, Building const * building_p)
{
	Box<double> boxA_l { building_p->_pos.x - building_p->_model._ray, building_p->_pos.x + building_p->_model._ray,
						building_p->_pos.y - building_p->_model._ray, building_p->_pos.y + building_p->_model._ray };
	Box<double> boxB_l { ent_p->_pos.x - ent_p->_model._ray, ent_p->_pos.x + ent_p->_model._ray,
						ent_p->_pos.y - ent_p->_model._ray, ent_p->_pos.y + ent_p->_model._ray };

	// Use 0.1 as a margin for range resource check
	Box<double> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX)-0.1,
						std::min(boxA_l._upperX, boxB_l._upperX),
						std::max(boxA_l._lowerY, boxB_l._lowerY)-0.1,
						std::min(boxA_l._upperY, boxB_l._upperY) };
	// Check intersections
	if(intersect_l._lowerX < intersect_l._upperX
	&& intersect_l._lowerY < intersect_l._upperY)
	{
		return true;
	}
	return false;
}

bool BuildingCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	Logger::getDebug() << "BuildingCommand:: apply Command "<<_source <<std::endl;
	MoveData const &moveData_l = *static_cast<MoveData const *>(data_p);
	Entity const * ent_l = state_p.getEntity(_source);
	Building const * building_l = static_cast<Building const *>(state_p.getEntity(_target));

	// if building is over stop
	if(building_l->_buildingProgress >= building_l->_buildingModel._buildingTime)
	{
		Logger::getDebug() << "BuildingCommand:: building over"<<std::endl;
		return true;
	}

	// If not in range et move
	if(!isInRange(state_p, ent_l, building_l))
	{
		Logger::getDebug() << "BuildingCommand:: moving"<<std::endl;
		// run move command
		_subMoveCommand.applyCommand(step_p, state_p, data_p);
	}
	// If in range build
	else
	{
		Logger::getDebug() << "BuildingCommand:: building"<<std::endl;
		step_p.addSteppable(new BuildingStep(_target, 1.));
	}

	// not over
	return false;
}

} // namespace octopus

