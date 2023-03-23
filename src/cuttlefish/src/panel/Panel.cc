#include "Panel.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace cuttlefish
{

Panel::Panel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p) :
	_x(x), _y(y), _icons(icons_p), _iconsPerLine(iconsPerLine_p)
{
	_background = new Picture(background_p, 276, 276, {1}, {1});
	_background->setDestination(x, y, 276, 276);
}

Panel::~Panel()
{
	delete _background;
	for(SpriteModel &sprite_l : _sprites)
	{
		delete sprite_l.sprite;
	}
}

void Panel::refresh(SpriteEntity const *sprite_p, octopus::State const &state_p)
{
	const octopus::Entity * selected_l = nullptr;
	if(sprite_p)
	{
		const octopus::Entity * cur_l = state_p.getEntity(sprite_p->getHandle());
		selected_l = cur_l;
	}

	// nothing to do
	if(selected_l == _lastSelection)
	{
		return;
	}

	_lastSelection = selected_l;

	for(SpriteModel &sprite_l : _sprites)
	{
		delete sprite_l.sprite;
	}
	_sprites.clear();

	if(selected_l == nullptr)
	{
		_grid.clear();
		return;
	}

	octopus::Player const &player_l = *state_p.getPlayer(_lastSelection->_player);

	// update
	if(_lastSelection->_model._isBuilder)
	{
		std::list<octopus::BuildingModel const *> buildingGrid_l = getAvailableBuildingModels(player_l);
		for(octopus::BuildingModel const * model_l : buildingGrid_l)
		{
			Picture *sprite_l = new Picture(_icons, 64, 64, {1}, {1});
			SpriteInfo const &info_l = _mapIcons.at(model_l->_id);
			sprite_l->setState(info_l.state);
			sprite_l->setFrame(info_l.frame);
			SpriteModel spriteModel_l;
			spriteModel_l.sprite = sprite_l;
			spriteModel_l.buildingModel = model_l;
			_sprites.push_back(spriteModel_l);
		}

	} else if(_lastSelection->_model._isBuilding)
	{
		std::list<octopus::UnitModel const *> unitGrid_l = getAvailableUnitModels(
			static_cast<octopus::BuildingModel const &>(_lastSelection->_model), player_l);
		for(octopus::UnitModel const * model_l : unitGrid_l)
		{
			Picture *sprite_l = new Picture(_icons, 64, 64, {1}, {1});
			SpriteInfo const &info_l = _mapIcons.at(model_l->_id);
			sprite_l->setState(info_l.state);
			sprite_l->setFrame(info_l.frame);
			SpriteModel spriteModel_l;
			spriteModel_l.sprite = sprite_l;
			spriteModel_l.unitModel = model_l;
			_sprites.push_back(spriteModel_l);
		}
	}

	// index used of position
	size_t idx_l = 0;
	_grid.clear();
	// update position of sprites
	for(SpriteModel & sprite_l : _sprites)
	{
		int x = idx_l % _iconsPerLine;
		int y = idx_l/_iconsPerLine;

		sprite_l.sprite->setDestination(_x + 4 + x * 68, _y + 4 + y * 68, 64, 64);
		_grid[{x,y}] = &sprite_l;
		++idx_l;
	}
}

void Panel::render(Window &window_p)
{
	_background->display(window_p);
	for(SpriteModel const & sprite_l : _sprites)
	{
		sprite_l.sprite->display(window_p);
	}
}

void Panel::addSpriteInfo(std::string const &model_p, int state_p, int frame_p)
{
	_mapIcons[model_p].state = state_p;
	_mapIcons[model_p].frame = frame_p;
}

SpriteModel const * Panel::getSpriteModel(Window &window_p, int x, int y) const
{
	for(SpriteModel const & sprite_l : _sprites)
	{
		if(sprite_l.sprite->isInside(window_p, x, y))
		{
			return &sprite_l;
		}
	}
	return nullptr;
}

SpriteModel const * Panel::getSpriteModelOnGrid(int x, int y) const
{
	auto it_l = _grid.find({x,y});
	if(it_l == _grid.end())
	{
		return nullptr;
	}
	return it_l->second;
}

} // namespace cuttlefish
