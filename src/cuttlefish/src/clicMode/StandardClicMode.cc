#include "StandardClicMode.hh"

// octopus
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/State.hh"

// cuttlefish
#include "texture/Texture.hh"
#include "panel/Panel.hh"
#include "window/Window.hh"
#include "world/World.hh"

namespace cuttlefish
{

StandardClicMode::~StandardClicMode()
{}

/// @brief handle mouse
/// @return true if the clic mode is over
bool StandardClicMode::handleMouseUp(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p)
{
	SpriteEntity * sprite_l = world_p.getSprite(window_p, e.button.x, e.button.y);

	// true if selection need to be added to current selection
	bool add_l = false;
	if(KMOD_SHIFT & SDL_GetModState())
	{
		add_l = true;
	}

	if(e.button.button == SDL_BUTTON_LEFT)
	{
		if(e.button.x != _downX
		|| e.button.y != _downY)
		{
			std::list<SpriteEntity *> newSelection_l = world_p.getSprites(window_p, _downX, _downY, e.button.x, e.button.y);

			// filter units if both units and buildings have been boxed
			std::list<SpriteEntity *> units_l;
			for(SpriteEntity * sprite_l : newSelection_l)
			{
				const octopus::Entity * cur_l = state_p.getEntity(sprite_l->getHandle());
				if(cur_l && cur_l->_model._isUnit)
				{
					units_l.push_back(sprite_l);
				}
			}

			if(!units_l.empty())
			{
				newSelection_l = units_l;
			}

			if(add_l)
			{
				addToSelection(selection_p, newSelection_l, state_p);
			}
			else
			{
				replaceSelection(selection_p, newSelection_l, state_p);
			}
		}
		else if(sprite_l)
		{
			if(add_l)
			{
				addToSelection(selection_p, {sprite_l}, state_p);
			}
			else
			{
				replaceSelection(selection_p, {sprite_l}, state_p);
			}
		}
		else
		{
			selection_p.clear();
		}
		panel_p.refresh(selection_p._sprite, state_p);
	}
	else if(e.button.button == SDL_BUTTON_RIGHT
	&& selection_p._sprite)
	{
		for(SpriteEntity * selected_l : selection_p._sprites)
		{
			const octopus::Entity * cur_l = state_p.getEntity(selected_l->getHandle());
			const octopus::Unit * unit_l = dynamic_cast<const octopus::Unit *>(cur_l);
			bool isStatic_l = cur_l->_model._isStatic;
			if(!isStatic_l)
			{
				const octopus::Entity * target_l = nullptr;
				const octopus::Building * targetBuilding_l = nullptr;
				if(sprite_l)
				{
					target_l = state_p.getEntity(sprite_l->getHandle());
					if(target_l->_model._isBuilding)
					{
						targetBuilding_l = dynamic_cast<const octopus::Building *>(target_l);
					}
				}
				if(target_l
				&& target_l->_model._isResource)
				{
					octopus::UnitHarvestCommand * command_l = new octopus::UnitHarvestCommand(
						selected_l->getHandle(),
						selected_l->getHandle(),
						sprite_l->getHandle(),
						window_p.getWorldVector(e.button.x, e.button.y),
						0,
						{window_p.getWorldVector(e.button.x, e.button.y)},
						true
					);
					controller_p.commitCommand(command_l);
				}
				else if(target_l
				&& target_l->_model._isBuilding
				&& !static_cast<const octopus::Building *>(target_l)->isBuilt())
				{
					octopus::EntityBuildingCommand * command_l = new octopus::EntityBuildingCommand(
						selected_l->getHandle(),
						selected_l->getHandle(),
						sprite_l->getHandle(),
						window_p.getWorldVector(e.button.x, e.button.y),
						0,
						{window_p.getWorldVector(e.button.x, e.button.y)},
						true
					);
					controller_p.commitCommand(command_l);
				}
				else if(target_l && unit_l
				&& targetBuilding_l
				&& targetBuilding_l->_buildingModel.isDeposit(unit_l->_typeOfResource)
				&& unit_l->_quantityOfResource > 0)
				{
					octopus::UnitDropCommand * command_l = new octopus::UnitDropCommand(
						selected_l->getHandle(),
						selected_l->getHandle(),
						sprite_l->getHandle(),
						window_p.getWorldVector(e.button.x, e.button.y),
						0,
						{window_p.getWorldVector(e.button.x, e.button.y)},
						true
					);
					controller_p.commitCommand(command_l);
				}
				else
				{
					octopus::EntityMoveCommand * command_l = new octopus::EntityMoveCommand(
						selected_l->getHandle(),
						selected_l->getHandle(),
						window_p.getWorldVector(e.button.x, e.button.y),
						0,
						{window_p.getWorldVector(e.button.x, e.button.y)},
						true
					);
					controller_p.commitCommand(command_l);
				}
			}
		}
	}
	_downX = -1;
	_downY = -1;

	return false;
}

/// @brief display sprites handled in this clic mode
void StandardClicMode::display(Window & window_p, double , int x, int y)
{
	if(_downX > 0 && _downY > 0)
	{
		int lx = std::min(x, _downX);
		int ly = std::min(y, _downY);
		int ux = std::max(x, _downX);
		int uy = std::max(y, _downY);
		SDL_Rect final_l {
			lx,
			ly,
			ux-lx,
			uy-ly
		};

		window_p.loadTexture("resources/background.png")->render(window_p.getRenderer(), final_l.x, final_l.y, final_l.h, final_l.w, nullptr);
	}
}

} // namespace cuttlefish

