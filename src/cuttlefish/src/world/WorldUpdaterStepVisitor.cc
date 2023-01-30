#include "WorldUpdaterStepVisitor.hh"

#include "World.hh"
#include "window/Window.hh"
#include "sprite/Sprite.hh"

// octopus
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/unit/UnitHarvestStep.hh"


namespace cuttlefish
{

void WorldUpdaterStepVisitor::spawn(octopus::Entity const &entity_p, octopus::Handle const &handle_p)
{
	if(handle_p >= _world._sprites.size())
	{
		_world._sprites.resize(handle_p+1, nullptr);
	}
	const octopus::EntityModel &model_l = entity_p._model;

	std::string texture_l = model_l._isStatic?"resources/square.png":"resources/circle.png";
	Sprite * sprite_l = new Sprite(handle_p, _window.loadTexture(texture_l), model_l._ray, 32, 32, 64, 64, {2, 2}, {0.25, 1});
	sprite_l->setPosition(entity_p._pos.x*32, entity_p._pos.y*32);

	_world._sprites[handle_p] = sprite_l;
	_world._listSprite.push_back(sprite_l);
}

void WorldUpdaterStepVisitor::clear(octopus::Handle const &handle_p)
{
	_world._listSprite.remove(_world._sprites[handle_p]);
	delete _world._sprites[handle_p];
	_world._sprites[handle_p] = nullptr;
}

void WorldUpdaterStepVisitor::visit(octopus::EntitySpawnStep const *step_p)
{
	spawn(step_p->getModel(), step_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::BuildingSpawnStep const *step_p)
{
	spawn(step_p->getModel(), step_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::ResourceSpawnStep const *step_p)
{
	spawn(step_p->getModel(), step_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::UnitSpawnStep const *step_p)
{
	spawn(step_p->getModel(), step_p->getHandle());
}

void WorldUpdaterStepVisitor::visit(octopus::UnitHarvestQuantityStep const *step_p)
{
	if(static_cast<octopus::Resource const *>(_state->getEntity(step_p->_res))->_resource <= 0.)
	{
		clear(step_p->_res);
	}

	// Update state of sprite harvesting
	_world._sprites[step_p->_handle]->setState(1);
}

void WorldUpdaterStepVisitor::visit(octopus::EntityHitPointChangeStep const *step_p)
{
	if(_state->getEntity(step_p->_handle)->_hp <= 0.)
	{
		clear(step_p->_handle);
	}
}

void WorldUpdaterStepVisitor::visit(octopus::EntityMoveStep const *step_p)
{
	_world._sprites[step_p->_handle]->move(step_p->_move.x*32, step_p->_move.y*32);
}

}
