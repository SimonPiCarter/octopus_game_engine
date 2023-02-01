#include "World.hh"
#include "WorldUpdaterStepVisitor.hh"
#include "sprite/Sprite.hh"
#include "sprite/SpriteLibrary.hh"

// octopus
#include "state/entity/Entity.hh"
#include "state/entity/Building.hh"
#include "state/State.hh"
#include "controller/Controller.hh"
#include "step/Step.hh"

namespace cuttlefish
{

void World::handleStep(Window &window_p, octopus::StateAndSteps const &steps_p, SpriteLibrary const &lib_p)
{
	WorldUpdaterStepVisitor vis_l(*this, window_p, steps_p._state, lib_p);

	if(_first)
	{
		_lastIt = steps_p._steps.begin();
		_first = false;

		// visit intial steps
		for(octopus::Steppable const * steppable_l : steps_p._initialStep.getSteppable())
		{
			vis_l(steppable_l);
		}
	}

	// Every step missing
	for(auto it_l = _lastIt ; it_l != steps_p._stepIt ; ++it_l)
	{
		for(Sprite * sprite_l : _listSprite)
		{
			// reset steps state
			octopus::Entity const &entity_l = *steps_p._state->getEntity(sprite_l->getHandle());
			if(entity_l._model._isBuilding
			&& static_cast<const octopus::Building &>(entity_l).isBlueprint())
			{
				sprite_l->setState(lib_p.getBlueprintState(entity_l._model._id));
			}
			else
			{
				sprite_l->setState(0);
			}
		}
		// Visit every stepapble in the step
		for(octopus::Steppable const * steppable_l : (*it_l)->getSteppable())
		{
			vis_l(steppable_l);
		}
	}
	_lastIt = steps_p._stepIt;
}

void World::display(Window &window_p, double elapsed_p)
{
	for(Sprite * sprite_l : _listSprite)
	{
		sprite_l->update(elapsed_p);
		sprite_l->render(window_p);
	}
}

Sprite * World::getSprite(Window &window_p, int x, int y) const
{
	for(Sprite * sprite_l : _listSprite)
	{
		if(sprite_l->isInside(window_p, x, y))
		{
			return sprite_l;
		}
	}
	return nullptr;
}

}
