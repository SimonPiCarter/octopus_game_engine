#include "ControllerStepVisitor.h"

#include "Controller.h"

// octopus
#include "command/data/AttackMoveData.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/State.hh"
#include "step/building/BuildingCancelStep.hh"
#include "step/building/BuildingStep.hh"
#include "step/command/harvest/CommandHarvestStep.hh"
#include "step/command/CommandWindUpDiffStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerAddOptionDivinityStep.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"
#include "step/unit/UnitHarvestStep.hh"


using octopus::to_double;

namespace godot
{

void ControllerStepVisitor::visit(octopus::EntitySpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::BuildingSpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::ResourceSpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::UnitSpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::BuildingCancelStep const *steppable_p)
{
	_controller.emit_signal("clear_entity", int(steppable_p->_handle));
}

void ControllerStepVisitor::visit(octopus::UnitHarvestQuantityStep const *steppable_p)
{
	_controller.emit_signal("harvest_unit", int(steppable_p->_handle));
	if(static_cast<octopus::Resource const *>(_state->getEntity(steppable_p->_res))->_resource <= 0.)
	{
		_controller.emit_signal("clear_entity", int(steppable_p->_res));
	}
}

void ControllerStepVisitor::visit(octopus::CommandHarvestTimeSinceHarvestStep const *steppable_p)
{
	// NA (ok)
}

void ControllerStepVisitor::visit(octopus::EntityHitPointChangeStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	double hp_l = ent_l->_hp;
	// warning when we have deleted it already (can happens when multiple change step in one step)
	_controller.hp_change(steppable_p->_handle, ent_l->_hp/ent_l->getHpMax());
	if(!ent_l->_alive)
	{
		_controller.kill(steppable_p->_handle);
	}
}

void ControllerStepVisitor::visit(octopus::EntityMoveStep const *steppable_p)
{
	_controller.move(steppable_p->_handle);
}

void ControllerStepVisitor::visit(octopus::BuildingStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	octopus::Building const * building_l = static_cast<octopus::Building const *>(ent_l);
	_controller.emit_signal("build", int(steppable_p->_handle), float(building_l->_buildingProgress/building_l->_buildingModel._buildingTime));
}

void ControllerStepVisitor::visit(octopus::PlayerAddOptionDivinityStep const *steppable_p)
{
	// NA (ok)
}

void ControllerStepVisitor::visit(octopus::PlayerAddOptionStep const *steppable_p)
{
	std::map<unsigned long, OptionManager> & map_l = _controller.getOptionManagers();
	if(map_l.find(steppable_p->_player) == map_l.end())
	{
		map_l.emplace(std::make_pair(steppable_p->_player, OptionManager(steppable_p->_player)));
	}
	map_l.at(steppable_p->_player).addOptionLayer(steppable_p);
	_controller.emit_signal("option_update");
}

void ControllerStepVisitor::visit(octopus::PlayerPopOptionStep const *steppable_p)
{
	_controller.getOptionManagers().at(steppable_p->_player).popOptionLayer(steppable_p);
	_controller.emit_signal("option_update");
}

void ControllerStepVisitor::visit(octopus::CommandWindUpDiffStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	if (!ent_l->getQueue().hasCurrentCommand())
	{
		return;
	}
	octopus::AttackMoveData const *data_l = dynamic_cast<octopus::AttackMoveData const *>(ent_l->getFrontQueue()._data);
	// set wind up state
	if(data_l && data_l->_windup == 1)
	{
		_controller.windup(steppable_p->_handle);
	}
}

void ControllerStepVisitor::visit(octopus::CustomStep const *steppable_p)
{
}

}
