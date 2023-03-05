#include "AttackMoveClicMode.hh"

// octopus
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "controller/Controller.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"

// cuttlefish
#include "sprite/SpriteEntity.hh"
#include "window/Window.hh"
#include "world/World.hh"

namespace cuttlefish
{

AttackMoveClicMode::~AttackMoveClicMode()
{}

/// @brief handle mouse
/// @return true if the clic mode is over
bool AttackMoveClicMode::handleMouseUp(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p)
{
	SpriteEntity * sprite_l = world_p.getSprite(window_p, e.button.x, e.button.y);

	if(e.button.button == SDL_BUTTON_LEFT)
	{
		for(SpriteEntity * selected_l : selection_p._sprites)
		{
			const octopus::Entity * cur_l = state_p.getEntity(selected_l->getHandle());
			const octopus::Player * player_l = state_p.getPlayer(cur_l->_player);
			if(cur_l->_model._isStatic)
			{
				continue;
			}

			const octopus::Entity * target_l = nullptr;
			const octopus::Player * targetPlayer_l = nullptr;
			if(sprite_l)
			{
				target_l = state_p.getEntity(sprite_l->getHandle());
				targetPlayer_l = state_p.getPlayer(target_l->_player);
			}

			// cannot directly attack resource
			if(target_l
			&& player_l->_team != targetPlayer_l->_team
			&& !target_l->_model._isResource)
			{
				octopus::EntityAttackCommand * command_l = new octopus::EntityAttackCommand(
					cur_l->_handle,
					cur_l->_handle,
					target_l->_handle
				);
				controller_p.commitCommand(command_l);
			}
			else
			{
				octopus::EntityAttackMoveCommand * command_l = new octopus::EntityAttackMoveCommand(
					cur_l->_handle,
					cur_l->_handle,
					window_p.getWorldVector(e.button.x, e.button.y),
					0,
					{window_p.getWorldVector(e.button.x, e.button.y)},
					true
				);
				controller_p.commitCommand(command_l);
			}
		}
		return true;
	}
	else if(e.button.button == SDL_BUTTON_RIGHT
	&& selection_p._sprite)
	{
		return true;
	}

	return false;
}

void AttackMoveClicMode::display(Window & window_p, double , int x, int y)
{
	// NA for now
}

} // namespace cuttlefish

