#include "Panel.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

#include "commands/CommandHotkeys.hh"

namespace cuttlefish
{

Panel::Panel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p) :
	_window(window_p), _x(x), _y(y), _icons(icons_p), _iconsPerLine(iconsPerLine_p)
{
	_background = new Picture(background_p, 276, 276, {1}, {1});
	_background->setDestination(x, y, 276, 276);
}

Panel::~Panel()
{
	delete _background;
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
	_sprites.clear();

	if(selected_l == nullptr)
	{
		_grid.clear();
		return;
	}

	octopus::Player const &player_l = *state_p.getPlayer(_lastSelection->_player);

	CommandHotkeys const * hkeys_l = CommandHotkeys::GetInstance();

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
			_sprites.push_back(CommandPicture());
			CommandPicture &commandPicture_l = _sprites.back();
			commandPicture_l._model = new SpriteModel();
			commandPicture_l._model->sprite = sprite_l;
			commandPicture_l._model->buildingModel = model_l;
			commandPicture_l._text = new Text(_window, {200,200,200}, 0, 0);
			commandPicture_l._text->setText(CommandHotkeys::getStringCode(hkeys_l->getKeycode(commandPicture_l)));
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
			_sprites.push_back(CommandPicture());
			CommandPicture &commandPicture_l = _sprites.back();
			commandPicture_l._model = new SpriteModel();
			commandPicture_l._model->sprite = sprite_l;
			commandPicture_l._model->unitModel = model_l;
			commandPicture_l._text = new Text(_window, {200,200,200}, 0, 0);
			commandPicture_l._text->setText(CommandHotkeys::getStringCode(hkeys_l->getKeycode(commandPicture_l)));
		}
	}

	// index used of position
	size_t idx_l = 0;
	_grid.clear();
	// update position of sprites
	for(CommandPicture & commandPicture_l : _sprites)
	{
		int x = idx_l % _iconsPerLine;
		int y = idx_l/_iconsPerLine;

		commandPicture_l._model->sprite->setDestination(_x + 4 + x * 68, _y + 4 + y * 68, 64, 64);
		commandPicture_l._text->setPosition(_x + 6 + x * 68, _y + 48 + y * 68);
		_grid[{x,y}] = &commandPicture_l;
		++idx_l;
	}

	int lastLine_l = 3;
	// add custom position buttons
	if(_lastSelection->_model._isBuilding && !dynamic_cast<octopus::Building const *>(_lastSelection)->isBuilt()
	|| _lastSelection->_model._isUnit)
	{
		Picture *sprite_l = new Picture(_icons, 64, 64, {1}, {1});
		sprite_l->setState(4);
		sprite_l->setFrame(0);
		_sprites.push_back(CommandPicture());
		CommandPicture &commandPicture_l = _sprites.back();
		commandPicture_l._model = new SpriteModel();
		commandPicture_l._model->sprite = sprite_l;
		commandPicture_l._type = CommandPicture::Type::Stop;
		commandPicture_l._model->sprite->setDestination(_x + 4 + (_iconsPerLine-1) * 68, _y + 4 + lastLine_l * 68, 64, 64);
		commandPicture_l._text = new Text(_window, {200,200,200}, _x + 6 + (_iconsPerLine-1) * 68, _y + 48 + lastLine_l * 68);
		commandPicture_l._text->setText(CommandHotkeys::getStringCode(hkeys_l->getKeycode(commandPicture_l)));
		_grid[{_iconsPerLine-1,3}] = &commandPicture_l;
	}
	if(_lastSelection->_model._isUnit)
	{
		Picture *sprite_l = new Picture(_icons, 64, 64, {1}, {1});
		sprite_l->setState(4);
		sprite_l->setFrame(1);
		_sprites.push_back(CommandPicture());
		CommandPicture &commandPicture_l = _sprites.back();
		commandPicture_l._model = new SpriteModel();
		commandPicture_l._model->sprite = sprite_l;
		commandPicture_l._type = CommandPicture::Type::AttackMove;
		commandPicture_l._model->sprite->setDestination(_x + 4 , _y + 4 + lastLine_l * 68, 64, 64);
		commandPicture_l._text = new Text(_window, {200,200,200}, _x + 6 , _y + 48 + lastLine_l * 68);
		commandPicture_l._text->setText(CommandHotkeys::getStringCode(hkeys_l->getKeycode(commandPicture_l)));
		_grid[{0,3}] = &commandPicture_l;
	}
}

void Panel::render(Window &window_p)
{
	_background->display(window_p);
	for(CommandPicture const & commandPicture_l : _sprites)
	{
		commandPicture_l._model->sprite->display(window_p);
		if(commandPicture_l._text)
		{
			commandPicture_l._text->display(window_p);
		}
	}
}

void Panel::addSpriteInfo(std::string const &model_p, int state_p, int frame_p)
{
	_mapIcons[model_p].state = state_p;
	_mapIcons[model_p].frame = frame_p;
}

CommandPicture const *Panel::getCommand(Window &window_p, int x, int y) const
{
	for(CommandPicture const & commandPicture_l : _sprites)
	{
		if(commandPicture_l._model->sprite->isInside(window_p, x, y))
		{
			return &commandPicture_l;
		}
	}
	return nullptr;
}

SpriteModel const * Panel::getSpriteModel(Sint32 const &key_p) const
{
	for(CommandPicture const &command_l : _sprites)
	{
		if(CommandHotkeys::GetInstance()->getKeycode(command_l) == key_p)
		{
			return command_l._model;
		}
	}
	return nullptr;
}

bool Panel::isStop(Sint32 const &key_p) const
{
	for(CommandPicture const &command_l : _sprites)
	{
		if(CommandHotkeys::GetInstance()->getKeycode(command_l) == key_p)
		{
			return command_l._type == CommandPicture::Type::Stop;
		}
	}
	return false;
}

bool Panel::isAttackMove(Sint32 const &key_p) const
{
	for(CommandPicture const &command_l : _sprites)
	{
		if(CommandHotkeys::GetInstance()->getKeycode(command_l) == key_p)
		{
			return command_l._type == CommandPicture::Type::AttackMove;
		}
	}
	return false;
}

SpriteModel const * Panel::getSpriteModelOnGrid(int x, int y) const
{
	auto it_l = _grid.find({x,y});
	if(it_l == _grid.end())
	{
		return nullptr;
	}
	return it_l->second->_model;
}

} // namespace cuttlefish
