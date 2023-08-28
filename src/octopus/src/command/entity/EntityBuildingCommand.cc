
#include "EntityBuildingCommand.hh"

#include <cmath>

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "step/Step.hh"
#include "step/building/BuildingStep.hh"
#include "step/building/BuildingCancelStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "utils/Box.hh"

namespace octopus
{

EntityBuildingCommand::EntityBuildingCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _target(target_p)
	, _subMoveCommand(commandHandle_p, source_p, finalPoint_p, gridStatus_p, waypoints_p, init_p)
{}

bool isInRange(State const &state_p, Entity const * ent_p, Building const * building_p)
{
	Box<Fixed> boxA_l { building_p->_pos.x - building_p->_model._ray, building_p->_pos.x + building_p->_model._ray,
						building_p->_pos.y - building_p->_model._ray, building_p->_pos.y + building_p->_model._ray };
	Box<Fixed> boxB_l { ent_p->_pos.x - ent_p->_model._ray, ent_p->_pos.x + ent_p->_model._ray,
						ent_p->_pos.y - ent_p->_model._ray, ent_p->_pos.y + ent_p->_model._ray };

	// Use 0.1 as a margin for range resource check
	Box<Fixed> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX)-0.1,
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

bool EntityBuildingCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const
{
	Logger::getDebug() << "EntityBuildingCommand:: apply Command "<<_source <<std::endl;
	MoveData const &moveData_l = *static_cast<MoveData const *>(data_p);
	Entity const * ent_l = state_p.getEntity(_source);

	// special case where entity died and has been replaced by another
	if(!state_p.hasEntity(_target))
	{
		Logger::getDebug() << "EntityBuildingCommand:: building died and has been replaced"<<std::endl;
		return true;
	}

	Building const * building_l = static_cast<Building const *>(state_p.getEntity(_target));

	if(ent_l->_player != building_l->_player)
	{
		Logger::getDebug() << "EntityBuildingCommand:: building other player building is not allowed"<<std::endl;
		return true;
	}
	if(!ent_l->_model._isBuilder)
	{
		Logger::getDebug() << "EntityBuildingCommand:: non builder cannot build"<<std::endl;
		return true;
	}

	if(building_l->_canceled)
	{
		Logger::getDebug() << "EntityBuildingCommand:: canceled"<<std::endl;
		return true;
	}

	if(building_l->isBlueprint()
	&& !building_l->_buildingModel.checkGrid(*building_l, state_p))
	{
		if(!step_p.isCanceled(_target))
		{
			Logger::getDebug() << "EntityBuildingCommand:: space taken and not cancelled yet"<<std::endl;
			step_p.addSteppable(new BuildingCancelStep(_target, building_l->_canceled, true));
			step_p.addSteppable(new PlayerSpendResourceStep(building_l->_player, getReverseCostMap(building_l->_model._cost)));
		}
		else
		{
			Logger::getDebug() << "EntityBuildingCommand:: space taken and already cancelled"<<std::endl;
		}
		return true;
	}

	// if building is over stop
	if(building_l->isBuilt())
	{
		Logger::getDebug() << "EntityBuildingCommand:: building over"<<std::endl;
		return true;
	}

	if(!building_l->_alive && !building_l->isBlueprint())
	{
		Logger::getDebug() << "EntityBuildingCommand:: building died"<<std::endl;
		return true;
	}

	// If not in range et move
	if(!isInRange(state_p, ent_l, building_l))
	{
		Logger::getDebug() << "EntityBuildingCommand:: moving"<<std::endl;
		// run move command
		_subMoveCommand.applyCommand(step_p, state_p, data_p, pathManager_p);
	}
	// If in range build after grid check
	else
	{
		Logger::getDebug() << "EntityBuildingCommand:: building"<<std::endl;
		step_p.addSteppable(new BuildingStep(_source, _target, ent_l->getProduction()));
	}

	// not over
	return false;
}

} // namespace octopus

