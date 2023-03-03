#include "StatsPanel.hh"

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

StatsPanel::StatsPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p, Selection &selection_p) :
	_x(x),
	_y(y),
	_icons(icons_p),
	_iconsPerLine(iconsPerLine_p),
	_textStats(window_p, x, y+80),
	_textQtyRes(window_p, x, y+185),
	_textResources(window_p, x, y+120),
	_selection(selection_p)
{
	_background = new Picture(background_p, 260./64., 0, 0, 400, 400, {1}, {1}, true);
	_background->setPosition(x, y);
	_textStats.addText("name", "", {0, 0, 0}, true);
	_textStats.addText("hp", "hp : ", {0, 0, 0}, false);
	_textStats.addText("hp_val", "", {0, 155, 0}, true);
	_textStats.addText("dmg", "dmg : ", {0, 0, 0}, false);
	_textStats.addText("dmg_val", "", {155, 0, 0}, true);
	_textStats.addText("armor", "armor : ", {0, 0, 0}, false);
	_textStats.addText("armor_val", "", {0, 0, 155}, true);

	_textQtyRes.addText("res_type", "",{0, 0, 0}, false);
	_textQtyRes.addText("spacer", " : ",{0, 0, 0}, false);
	_textQtyRes.addText("qty_val", "",{0, 0, 0}, false);

	_textResources.addText("res_type", "", {0, 0, 0}, true);
	_textResources.addText("qty", "quantity : ", {0, 0, 0}, false);
	_textResources.addText("qty_val", "", {0, 0, 0}, true);
}

StatsPanel::~StatsPanel()
{
	delete _background;
	for(SpriteModel &sprite_l : _sprites)
	{
		delete sprite_l.sprite;
	}
}

void StatsPanel::refresh(Window &window_p, octopus::State const &state_p)
{
	// nothing to do
	SelectionKey curKey_l = _selection.key();
	if(_lastKey == curKey_l)
	{
		return;
	}

	// update key
	std::swap(_lastKey, curKey_l);

	// clean up old panel
	for(SpriteModel &sprite_l : _sprites)
	{
		delete sprite_l.sprite;
	}
	_sprites.clear();
	_texts.clear();

	// if nothing selected just return
	if(_selection._sprites.empty())
	{
		return;
	}


	if(_selection._sprites.size() == 1)
	{
		// set up stats panel
		_monoSelection = state_p.getEntity(_selection._sprite->getHandle());
	}
	else
	{
		_monoSelection = nullptr;
		// set up group selection panel
		for(auto &&pair_l : _selection._spritesPerModel)
		{
			std::string const &id_l = pair_l.first;
			std::set<Sprite *, SpriteComparator> const &set_l = pair_l.second;
			if(set_l.empty())
			{
				continue;
			}
			octopus::Entity const * ent_l = state_p.getEntity((*set_l.begin())->getHandle());

			Picture *sprite_l = new Picture(_icons, 1, 0, 0, 64, 64, {1}, {1}, true);
			SpriteInfo const &info_l = _mapIcons.at(ent_l->_model._id);
			sprite_l->setState(info_l.state);
			sprite_l->setFrame(info_l.frame);

			SpriteModel spriteModel_l;
			spriteModel_l.sprite = sprite_l;
			_sprites.push_back(spriteModel_l);

			_texts.emplace_back(&window_p, SDL_Color {0, 0, 0}, 0, 0);
			_texts.back().setText(std::to_string(set_l.size()));
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

		sprite_l.sprite->setPosition(_x + x * 65, _y + y * 65);
		_grid[{x,y}] = &sprite_l;
		++idx_l;
	}
	idx_l = 0;
	// update position of texts
	for(Text & text_l : _texts)
	{
		int x = idx_l % _iconsPerLine;
		int y = idx_l/_iconsPerLine;

		text_l.setPosition(_x + x * 65, _y + y * 65);
		++idx_l;
	}
}

void StatsPanel::render(Window &window_p)
{
	_background->render(window_p);
	for(SpriteModel const & sprite_l : _sprites)
	{
		sprite_l.sprite->render(window_p);
	}
	for(Text const & text_l : _texts)
	{
		text_l.display(window_p);
	}

	if(_monoSelection)
	{
		if (_monoSelection->_model._isUnit || _monoSelection->_model._isBuilding)
		{
			_textStats.updateText("name", _monoSelection->_model._id);
			std::stringstream ss_l;
			ss_l<<_monoSelection->_hp<<"/"<<_monoSelection->_model._hpMax;
			_textStats.updateText("hp_val", ss_l.str());

			ss_l.str("");
			ss_l<<_monoSelection->getDamageNoBonus();
			_textStats.updateText("dmg_val", ss_l.str());

			ss_l.str("");
			ss_l<<_monoSelection->getArmor();
			_textStats.updateText("armor_val", ss_l.str());

			// display stats on selection
			_textStats.display(window_p);

			if(_monoSelection->_model._isUnit)
			{
				octopus::Unit const *unit_l = static_cast<octopus::Unit const *>(_monoSelection);
				if(unit_l->_quantityOfResource > 1e-5)
				{
					std::stringstream ss_l;
					ss_l<<std::abs(std::ceil(unit_l->_quantityOfResource-1e-5));
					_textQtyRes.updateText("qty_val", ss_l.str());
					_textQtyRes.updateText("res_type", to_string(unit_l->_typeOfResource));
					_textQtyRes.display(window_p);
				}
			}

		}
		else if (_monoSelection->_model._isResource)
		{
			octopus::Resource const * resource_l = dynamic_cast<octopus::Resource const *>(_monoSelection);
			std::stringstream ss_l;
			ss_l<<std::abs(std::ceil(resource_l->_resource-1e-5));
			_textResources.updateText("qty_val", ss_l.str());

			_textResources.updateText("res_type", to_string(resource_l->_type));

			// display stats on selection
			_textResources.display(window_p);
		}
	}
}

void StatsPanel::addSpriteInfo(std::string const &model_p, int state_p, int frame_p)
{
	_mapIcons[model_p].state = state_p;
	_mapIcons[model_p].frame = frame_p;
}

SpriteModel const * StatsPanel::getSpriteModel(Window &window_p, int x, int y) const
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

SpriteModel const * StatsPanel::getSpriteModelOnGrid(int x, int y) const
{
	auto it_l = _grid.find({x,y});
	if(it_l == _grid.end())
	{
		return nullptr;
	}
	return it_l->second;
}

} // namespace cuttlefish
