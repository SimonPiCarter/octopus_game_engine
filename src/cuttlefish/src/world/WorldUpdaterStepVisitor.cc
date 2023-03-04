#include "WorldUpdaterStepVisitor.hh"

#include "logger/Logger.hh"
#include "World.hh"
#include "window/Window.hh"
#include "sprite/SpriteEntity.hh"
#include "sprite/SpriteLibrary.hh"
#include "panel/Panel.hh"
#include "panel/DivinityPanel.hh"

// octopus
#include "command/data/AttackMoveData.hh"
#include "state/entity/Entity.hh"
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
#include "step/unit/UnitHarvestStep.hh"


using octopus::to_double;

namespace cuttlefish
{

void WorldUpdaterStepVisitor::spawn(octopus::Handle const &handle_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
	if(handle_p >= _world._sprites.size())
	{
		_world._sprites.resize(handle_p+1, nullptr);
	}
	const octopus::EntityModel &model_l = entity_l._model;

	SpriteEntity * sprite_l = _lib.createSpriteEntity(handle_p, model_l._id, false);
	sprite_l->setPosition(to_double(entity_l._pos.x)*32., to_double(entity_l._pos.y)*32.);

	_world._sprites[handle_p] = sprite_l;
	_world._listSprite.push_back(sprite_l);
}

void WorldUpdaterStepVisitor::clear(octopus::Handle const &handle_p)
{
	// check if already cleared up
	if(_world._sprites[handle_p] == nullptr)
	{
		return;
	}
	// update selection
	_world.clearSpriteFromSelections(_world._sprites[handle_p], *_state);
	_panel.refresh(_world.getSelection()._sprite, *_state);

	// remove sprite
	_world._listSprite.remove(_world._sprites[handle_p]);
	delete _world._sprites[handle_p];
	_world._sprites[handle_p] = nullptr;
}

void WorldUpdaterStepVisitor::visit(octopus::EntitySpawnStep const *steppable_p)
{
	spawn(steppable_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::BuildingSpawnStep const *steppable_p)
{
	spawn(steppable_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::ResourceSpawnStep const *steppable_p)
{
	spawn(steppable_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::UnitSpawnStep const *steppable_p)
{
	spawn(steppable_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::BuildingCancelStep const *steppable_p)
{
	clear(steppable_p->_handle);
}

void WorldUpdaterStepVisitor::visit(octopus::UnitHarvestQuantityStep const *steppable_p)
{
	if(static_cast<octopus::Resource const *>(_state->getEntity(steppable_p->_res))->_resource <= 0.)
	{
		clear(steppable_p->_res);
	}
}

void WorldUpdaterStepVisitor::visit(octopus::CommandHarvestTimeSinceHarvestStep const *steppable_p)
{
	if(_world._sprites[steppable_p->_handle])
	{
		// Update state of sprite harvesting
		_world._sprites[steppable_p->_handle]->setStateNoReset(1);
	}
}

void WorldUpdaterStepVisitor::visit(octopus::EntityHitPointChangeStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	double hp_l = ent_l->_hp;
	if(hp_l <= 0.)
	{
		clear(steppable_p->_handle);
	}
	else
	{
		_world._sprites[steppable_p->_handle]->setLifePercent(100*hp_l/ent_l->_model._hpMax);
	}
}

void WorldUpdaterStepVisitor::visit(octopus::EntityMoveStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	if(_world._sprites[steppable_p->_handle])
	{
		_world._sprites[steppable_p->_handle]->setPosition(to_double(ent_l->_pos.x)*32, to_double(ent_l->_pos.y)*32);
	}
}

void WorldUpdaterStepVisitor::visit(octopus::BuildingStep const *steppable_p)
{
	if(_world._sprites[steppable_p->_handle])
	{
		// Update state of sprite harvesting
		_world._sprites[steppable_p->_handle]->setStateNoReset(2);
	}
}

void WorldUpdaterStepVisitor::visit(octopus::PlayerAddOptionDivinityStep const *steppable_p)
{
	_divPanel.addOptionLayer(steppable_p->_player, steppable_p->_types);
}

void WorldUpdaterStepVisitor::visit(octopus::CommandWindUpDiffStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	octopus::AttackMoveData const *data_l = dynamic_cast<octopus::AttackMoveData const *>(ent_l->getFrontQueue()._data);
	if(_world._sprites[steppable_p->_handle])
	{
		// set wind up state
		if(data_l && data_l->_windup == 1)
		{
			_world._sprites[steppable_p->_handle]->setState(1);
			_world._sprites[steppable_p->_handle]->queueState(0);
		}
	}
}

}
