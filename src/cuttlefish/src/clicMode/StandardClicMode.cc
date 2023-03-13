#include "StandardClicMode.hh"

// octopus
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/player/Player.hh"
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
	SpriteEntity * sprite_l = world_p.getSprite(window_p, e.button.x, e.button.y, &state_p);

	// true if selection need to be added to current selection
	bool add_l = false;
	if(KMOD_SHIFT & SDL_GetModState())
	{
		add_l = true;
	}
	bool all_l = false;
	if(KMOD_CTRL & SDL_GetModState())
	{
		all_l = true;
	}

	if(e.button.button == SDL_BUTTON_LEFT)
	{
		// box selection
		if(isBoxEnough(e.button.x , e.button.y))
		{
			std::list<SpriteEntity *> newSelection_l = world_p.getSprites(window_p, _downX, _downY, e.button.x, e.button.y, &state_p);

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
			std::list<SpriteEntity *> sprites_l = {sprite_l};
			octopus::Entity const * ref_l = state_p.getEntity(sprite_l->getHandle());
			// check for double clic
			if((isDoubleClic() && _lastCickedSprite == sprite_l)
			|| all_l)
			{
				// add every same player same model to selection
				std::list<SpriteEntity *> allSprites_l = world_p.getSprites(window_p, 0, 0, window_p.getWidth(), window_p.getHeight(), &state_p);
				// check for model
				for(SpriteEntity * spriteEnt_l : allSprites_l)
				{
					octopus::Entity const * ent_l = state_p.getEntity(spriteEnt_l->getHandle());
					// compare pointer for faster comparison
					if(&ent_l->_model == &ref_l->_model
					&& ent_l->_player == ref_l->_player
					&& ent_l != ref_l)
					{
						sprites_l.push_back(spriteEnt_l);
					}
				}
			}
			if(add_l)
			{
				addToSelection(selection_p, sprites_l, state_p);
			}
			else
			{
				replaceSelection(selection_p, sprites_l, state_p);
			}
			_lastCickedSprite = sprite_l;
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
		handleRightClic(window_p.getWorldVector(e.button.x, e.button.y), selection_p, world_p, panel_p, window_p, state_p, controller_p, sprite_l);
	}
	_downX = -1;
	_downY = -1;

	return false;
}

/// @brief display sprites handled in this clic mode
void StandardClicMode::display(Window & window_p, double , int x, int y)
{
	if(_downX > 0 && _downY > 0 && isBoxEnough(x, y))
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

void StandardClicMode::handleRightClic(octopus::Vector const &clicWorldPosition_p, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p, SpriteEntity * sprite_p)
{
	// mouse tick
	Picture * tick_l = new Picture(window_p.loadTexture("resources/clic_marker.png"), 16, 16, {5}, {0.1});
	tick_l->setEndAfterLastFrame(true);
	octopus::Vector pixelPos_l = window_p.getPixelVector(to_double(clicWorldPosition_p.x), to_double(clicWorldPosition_p.y));
	tick_l->setDestination(pixelPos_l.x.to_int()-16-window_p.getCamera().x, pixelPos_l.y.to_int()-16-window_p.getCamera().y, 32, 32);
	world_p.getDyingPictures().push_back(tick_l);

	for(SpriteEntity * selected_l : selection_p._sprites)
	{
		const octopus::Entity * cur_l = state_p.getEntity(selected_l->getHandle());
		const octopus::Unit * unit_l = dynamic_cast<const octopus::Unit *>(cur_l);
		const octopus::Player * player_l = state_p.getPlayer(cur_l->_player);
		bool isStatic_l = cur_l->_model._isStatic;
		if(!isStatic_l)
		{
			const octopus::Entity * target_l = nullptr;
			const octopus::Building * targetBuilding_l = nullptr;
			const octopus::Player * targetPlayer_l = nullptr;
			if(sprite_p)
			{
				target_l = state_p.getEntity(sprite_p->getHandle());
				if(target_l->_model._isBuilding)
				{
					targetBuilding_l = dynamic_cast<const octopus::Building *>(target_l);
				}
				targetPlayer_l = state_p.getPlayer(target_l->_player);
			}
			if(target_l
			&& target_l->_model._isResource)
			{
				octopus::UnitHarvestCommand * command_l = new octopus::UnitHarvestCommand(
					selected_l->getHandle(),
					selected_l->getHandle(),
					sprite_p->getHandle(),
					clicWorldPosition_p,
					0,
					{clicWorldPosition_p},
					true
				);
				controller_p.commitCommandAsPlayer(command_l, world_p.getPlayer());
			}
			else if(target_l
			&& player_l->_team != targetPlayer_l->_team)
			{
				octopus::EntityAttackCommand * command_l = new octopus::EntityAttackCommand(
					selected_l->getHandle(),
					selected_l->getHandle(),
					sprite_p->getHandle(),
					true
				);
				controller_p.commitCommandAsPlayer(command_l, world_p.getPlayer());
			}
			else if(target_l
			&& target_l->_model._isBuilding
			&& !static_cast<const octopus::Building *>(target_l)->isBuilt())
			{
				octopus::EntityBuildingCommand * command_l = new octopus::EntityBuildingCommand(
					selected_l->getHandle(),
					selected_l->getHandle(),
					sprite_p->getHandle(),
					clicWorldPosition_p,
					0,
					{clicWorldPosition_p},
					true
				);
				controller_p.commitCommandAsPlayer(command_l, world_p.getPlayer());
			}
			else if(target_l && unit_l
			&& targetBuilding_l
			&& targetBuilding_l->_buildingModel.isDeposit(unit_l->_typeOfResource)
			&& unit_l->_quantityOfResource > 0)
			{
				octopus::UnitDropCommand * command_l = new octopus::UnitDropCommand(
					selected_l->getHandle(),
					selected_l->getHandle(),
					sprite_p->getHandle(),
					clicWorldPosition_p,
					0,
					{clicWorldPosition_p},
					true
				);
				controller_p.commitCommandAsPlayer(command_l, world_p.getPlayer());
			}
			else
			{
				octopus::EntityMoveCommand * command_l = new octopus::EntityMoveCommand(
					selected_l->getHandle(),
					selected_l->getHandle(),
					clicWorldPosition_p,
					0,
					{clicWorldPosition_p},
					true
				);
				controller_p.commitCommandAsPlayer(command_l, world_p.getPlayer());
			}
		}
	}
}

bool StandardClicMode::isBoxEnough(int x, int y) const
{
	int lx = std::min(x, _downX);
	int ly = std::min(y, _downY);
	int ux = std::max(x, _downX);
	int uy = std::max(y, _downY);

	return ux-lx > 5
		|| uy-ly > 5;
}

bool StandardClicMode::isDoubleClic()
{
	auto cur_l = std::chrono::steady_clock::now();
	std::chrono::duration<long long, std::nano> elapsed_nano_seconds_l = cur_l - lastCallForDoubleClic_l;

	/// should be 0.5 seconds
	bool isIt_l = elapsed_nano_seconds_l.count() < 500000000;
	lastCallForDoubleClic_l = cur_l;

	return isIt_l;
}


} // namespace cuttlefish

