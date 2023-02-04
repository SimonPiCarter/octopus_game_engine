#include "StandardClicMode.hh"

// octopus
#include "command/unit/UnitHarvestCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"

// cuttlefish
#include "panel/Panel.hh"
#include "window/Window.hh"
#include "world/World.hh"

namespace cuttlefish
{

StandardClicMode::~StandardClicMode()
{}

/// @brief handle mouse
/// @return true if the clic mode is over
bool StandardClicMode::handleMouse(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p)
{
	Sprite * sprite_l = world_p.getSprite(window_p, e.button.x, e.button.y);

	if(e.button.button == SDL_BUTTON_LEFT)
	{
		selection_p._sprite = sprite_l;
		if(selection_p._sprite)
		{
			const octopus::Entity * cur_l = state_p.getEntity(selection_p._sprite->getHandle());
			panel_p.refresh(cur_l, state_p);
		}
		else
		{
			panel_p.refresh(nullptr, state_p);
		}
	}
	else if(e.button.button == SDL_BUTTON_RIGHT
	&& selection_p._sprite)
	{
		const octopus::Entity * cur_l = state_p.getEntity(selection_p._sprite->getHandle());
		bool isStatic_l = cur_l->_model._isStatic;
		if(!isStatic_l)
		{
			const octopus::Entity * target_l = nullptr;
			if(sprite_l)
			{
				target_l = state_p.getEntity(sprite_l->getHandle());
			}
			if(target_l
			&& target_l->_model._isResource)
			{
				octopus::UnitHarvestCommand * command_l = new octopus::UnitHarvestCommand(
					selection_p._sprite->getHandle(),
					selection_p._sprite->getHandle(),
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
					selection_p._sprite->getHandle(),
					selection_p._sprite->getHandle(),
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
					selection_p._sprite->getHandle(),
					selection_p._sprite->getHandle(),
					window_p.getWorldVector(e.button.x, e.button.y),
					0,
					{window_p.getWorldVector(e.button.x, e.button.y)},
					true
				);
				controller_p.commitCommand(command_l);
			}
		}
	}

	return false;
}

/// @brief display sprites handled in this clic mode
void StandardClicMode::display(Window &, double , int, int )
{

}

} // namespace cuttlefish

