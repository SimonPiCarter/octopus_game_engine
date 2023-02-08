#include "World.hh"
#include "WorldUpdaterStepVisitor.hh"
#include "sprite/Sprite.hh"
#include "sprite/SpriteLibrary.hh"

#include <SDL2/SDL.h>

// octopus
#include "state/entity/Entity.hh"
#include "state/entity/Building.hh"
#include "state/State.hh"
#include "controller/Controller.hh"
#include "step/Step.hh"
#include "window/Window.hh"

namespace cuttlefish
{

void World::handleStep(Window &window_p, Panel &panel_p, DivinityPanel &divPanel_p, octopus::StateAndSteps const &steps_p, SpriteLibrary const &lib_p)
{
	WorldUpdaterStepVisitor vis_l(*this, window_p, panel_p, divPanel_p, steps_p._state, lib_p);

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
			sprite_l->setStateNoReset(0);

			octopus::Entity const &entity_l = *steps_p._state->getEntity(sprite_l->getHandle());
			if(entity_l._model._isBuilding
			&& static_cast<const octopus::Building &>(entity_l).isBlueprint())
			{
				sprite_l->setStateNoReset(lib_p.getBlueprintState(entity_l._model._id));
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

Sprite * World::getSprite(Window const &window_p, int x, int y) const
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

std::list<Sprite *> World::getSprites(Window const &window_p, int lx, int ly, int ux, int uy) const
{
	std::list<Sprite *> list_l;

	for(Sprite * sprite_l : _listSprite)
	{
		SDL_Point const &cam_l = window_p.getCamera();
		if(sprite_l->intersect(lx + cam_l.x, ly + cam_l.y, ux + cam_l.x, uy + cam_l.y))
		{
			list_l.push_back(sprite_l);
		}
	}

	return list_l;
}

}
