#include "BuildClicMode.hh"

// octopus
#include "command/building/BuildingBlueprintCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/State.hh"

// cuttlefish
#include "panel/Panel.hh"
#include "window/Window.hh"
#include "world/World.hh"
#include "sprite/SpriteLibrary.hh"

namespace cuttlefish
{

BuildClicMode::BuildClicMode(octopus::BuildingModel const &model_p, SpriteLibrary const &lib_p) : _model(model_p)
{
	_sprite = lib_p.createSprite(model_p._id, false);
	if(lib_p.getBlueprintState(model_p._id) > 0)
	{
		_sprite->setState(lib_p.getBlueprintState(model_p._id));
	}
}

BuildClicMode::~BuildClicMode()
{
	delete _sprite;
}

/// @brief handle mouse
/// @return true if the clic mode is over
bool BuildClicMode::handleMouseUp(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p)
{
	if(e.button.button == SDL_BUTTON_RIGHT)
	{
		return true;
	} else if(e.button.button == SDL_BUTTON_LEFT)
	{
		SDL_Point const & cam_l = window_p.getCamera();
		int newX_l = e.button.x + cam_l.x;
		int newY_l = e.button.y + cam_l.y;
		const octopus::Entity * cur_l = state_p.getEntity(selection_p._sprite->getHandle());

		octopus::BuildingBlueprintCommand * command_l = new octopus::BuildingBlueprintCommand(
			{double(newX_l/32), double(newY_l/32)},
			cur_l->_player,
			_model
		);
		controller_p.commitCommand(command_l);
		return true;
	}

	return false;
}

/// @brief display sprites handled in this clic mode
void BuildClicMode::display(Window & window_p, double elapsed_p, int x, int y)
{
	SDL_Point const & cam_l = window_p.getCamera();
	int newX_l = x + cam_l.x;
	int newY_l = y + cam_l.y;
	_sprite->setPosition(newX_l - (newX_l%32), newY_l - (newY_l%32));
	_sprite->update(elapsed_p);
	_sprite->render(window_p);
}

} // namespace cuttlefish

