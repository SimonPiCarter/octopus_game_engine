#include "Panel.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace cuttlefish
{

Panel::Panel(int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p) :
	_x(x), _y(y), _icons(icons_p), _iconsPerLine(iconsPerLine_p)
{
	_background = new Sprite(0, background_p, 0.5, 0, 0, 400, 400, {1}, {1}, true);
	_background->setPosition(x, y);
}

void Panel::refresh(octopus::Entity const *selected_p, octopus::State const &state_p)
{
	// nothing to do
	if(selected_p == _lastSelection)
	{
		return;
	}

	_lastSelection = selected_p;

	for(Sprite * sprite_l : _sprites)
	{
		delete sprite_l;
	}
	_sprites.clear();

	if(selected_p == nullptr)
	{
		return;
	}

	octopus::Player const &player_l = *state_p.getPlayer(_lastSelection->_player);

	// update
	if(_lastSelection->_model._isUnit)
	{
		std::list<octopus::BuildingModel const *> buildingGrid_l = getAvailableBuildingModels(player_l);
		for(octopus::BuildingModel const * model_l : buildingGrid_l)
		{
			Sprite *sprite_l = new Sprite(0, _icons, 1, 0, 0, 64, 64, {1}, {1}, true);
			SpriteInfo const &info_l = _mapIcons.at(model_l->_id);
			sprite_l->setState(info_l.state);
			sprite_l->setFrame(info_l.frame);
			_sprites.push_back(sprite_l);
		}

	} else if(_lastSelection->_model._isBuilding)
	{
		std::list<octopus::UnitModel const *> unitGrid_l = getAvailableUnitModels(
			static_cast<octopus::BuildingModel const &>(_lastSelection->_model), player_l);
		for(octopus::UnitModel const * model_l : unitGrid_l)
		{
			Sprite *sprite_l = new Sprite(0, _icons, 1, 0, 0, 64, 64, {1}, {1}, true);
			SpriteInfo const &info_l = _mapIcons.at(model_l->_id);
			sprite_l->setState(info_l.state);
			sprite_l->setFrame(info_l.frame);
			_sprites.push_back(sprite_l);
		}
	}

	// index used of position
	size_t idx_l = 0;
	// update position of sprites
	for(Sprite * sprite_l : _sprites)
	{
		int x = idx_l % _iconsPerLine;
		int y = idx_l/_iconsPerLine;

		sprite_l->setPosition(_x + x * 65, _y + y * 65);

		++idx_l;
	}
}

void Panel::render(Window &window_p) const
{
	_background->render(window_p);
	for(Sprite * sprite_l : _sprites)
	{
		sprite_l->render(window_p);
	}
}

void Panel::addSpriteInfo(std::string const &model_p, int state_p, int frame_p)
{
	_mapIcons[model_p].state = state_p;
	_mapIcons[model_p].frame = frame_p;
}

} // namespace cuttlefish
