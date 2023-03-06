#include "World.hh"
#include "WorldUpdaterStepVisitor.hh"
#include "sprite/SpriteEntity.hh"
#include "sprite/SpriteLibrary.hh"

#include <SDL2/SDL.h>

// octopus
#include "state/entity/Entity.hh"
#include "state/entity/Building.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "controller/Controller.hh"
#include "step/Step.hh"
#include "window/Window.hh"

namespace cuttlefish
{

void World::handleStep(Window &window_p, Panel &panel_p, StatsPanel &statsPanel_p, DivinityPanel &divPanel_p, octopus::StateAndSteps const &steps_p, SpriteLibrary const &lib_p)
{
	WorldUpdaterStepVisitor vis_l(*this, window_p, panel_p, statsPanel_p, divPanel_p, steps_p._state, lib_p);

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
		for(SpriteEntity * sprite_l : _listSprite)
		{
			// reset steps state
			if(!sprite_l->hasStateQueued())
			{
				sprite_l->setStateNoReset(0);
			}

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

void World::display(Window &window_p, octopus::State const &state_p, double elapsed_p)
{
	octopus::Player const * player_l = state_p.getPlayer(getPlayer());
	std::map<long, std::list<SpriteEntity *> > mapSprite_l;
	for(SpriteEntity * sprite_l : _listSprite)
	{
		sprite_l->update(elapsed_p);

		octopus::Entity const &ent_l = *state_p.getEntity(sprite_l->getHandle());
		// dispay if not unit or if in vision range
		if(!ent_l._model._isUnit
		|| state_p.getVisionHandler().isVisible(player_l->_team, ent_l))
		{
			mapSprite_l[long(sprite_l->getY())].push_back(sprite_l);
		}
	}

	for(auto &&pair_l : mapSprite_l)
	{
		std::list<SpriteEntity*> const &list_l = pair_l.second;
		for(SpriteEntity * sprite_l : list_l)
		{
			sprite_l->render(window_p);
		}
	}

	for(auto &&pair_l : mapSprite_l)
	{
		std::list<SpriteEntity*> const &list_l = pair_l.second;
		for(SpriteEntity * sprite_l : list_l)
		{
			sprite_l->renderLifeBar(window_p, elapsed_p);
		}
	}
}

SpriteEntity * World::getSprite(Window const &window_p, int x, int y, octopus::State const *state_p) const
{
	octopus::Player const *player_l = state_p?state_p->getPlayer(_player):nullptr;

	SpriteEntity * return_l = nullptr;

	for(SpriteEntity * sprite_l : _listSprite)
	{
		bool visibilyCheck_l = !state_p || state_p->getVisionHandler().isVisible(player_l->_team, *state_p->getEntity(sprite_l->getHandle()));
		if(visibilyCheck_l && sprite_l->isInside(window_p, x, y))
		{
			return_l = sprite_l;
		}
	}
	return return_l;
}

std::list<SpriteEntity *> World::getSprites(Window const &window_p, int lx, int ly, int ux, int uy, octopus::State const *state_p) const
{
	octopus::Player const *player_l = state_p?state_p->getPlayer(_player):nullptr;

	std::list<SpriteEntity *> list_l;

	for(SpriteEntity * sprite_l : _listSprite)
	{
		bool visibilyCheck_l = !state_p || state_p->getVisionHandler().isVisible(player_l->_team, *state_p->getEntity(sprite_l->getHandle()));
		if(visibilyCheck_l && sprite_l->intersect(lx, ly, ux, uy))
		{
			list_l.push_back(sprite_l);
		}
	}

	return list_l;
}

void World::addSelection(unsigned long idx_p)
{
	addToSelection(_selections[idx_p], _selection);
}

void World::registerSelection(unsigned long idx_p)
{
	_selections[idx_p] = _selection;
}

void World::useSelection(unsigned long idx_p)
{
	if(_selections.find(idx_p) != _selections.end())
	{
		_selection = _selections[idx_p];
	}
}

void World::clearSpriteFromSelections(SpriteEntity * sprite_p, octopus::State const &state_p)
{
	getSelection().removeSprite(sprite_p, state_p);
	for(auto &&pair_l : _selections)
	{
		pair_l.second.removeSprite(sprite_p, state_p);
	}
}

std::list<SpriteEntity *> const &World::getListSprite() const
{
	return _listSprite;
}

}
