#include "StatsPanel.hh"

#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/CommandQueue.hh"
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

StatsPanel::StatsPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, Texture const *barBack_p, Texture const *barFill_p, int iconsPerLine_p, Selection &selection_p) :
	_x(x),
	_y(y),
	_icons(icons_p),
	_iconsPerLine(iconsPerLine_p),
	_textStats(window_p, x, y+5),
	_textResources(window_p, x, y+45),
	_textQtyRes(window_p, x+150, y+45),
	_selection(selection_p)
{
	_background = new Picture(background_p, 400, 400, {1}, {1});
	_background->setDestination(x, y, 261, 261);

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

	if(barBack_p && barFill_p)
	{
		for(size_t idx_l = 0 ; idx_l < 2*_iconsPerLine ; ++ idx_l)
		{
			int x = idx_l % _iconsPerLine;
			int y = idx_l/_iconsPerLine;

			_productionPictures.push_back(new ProductionPicture());
			_productionPictures.back()->_icon = new Picture(_icons, 64, 64, {1}, {1});
			_productionPictures.back()->_bar = new ProgressBar(new Picture(barBack_p, 62, 6, {1}, {1}), new Picture(barFill_p, 62, 6, {1}, {1}), 62, 6, 1);

			int posX_l = _x + 1 + x * 65;
			// add offset to be at bottom of the panel
			int posY_l = _y + 120 + y * 65;
			_productionPictures.back()->_icon->setDestination(posX_l, posY_l, 64, 64);
			// Pos + offset to have progress bar on the bottom
			_productionPictures.back()->_bar->setPosition(posX_l, posY_l + 56);
		}
	}
}

StatsPanel::~StatsPanel()
{
	for(ProductionPicture *ptr_l : _productionPictures)
	{
		delete ptr_l;
	}
	delete _background;
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
	for(SelectionPicture *ptr_l : _selectionPictures)
	{
		delete ptr_l;
	}
	_selectionPictures.clear();

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
			std::set<SpriteEntity *, SpriteComparator> const &set_l = pair_l.second;
			if(set_l.empty())
			{
				continue;
			}
			octopus::Entity const * ent_l = state_p.getEntity((*set_l.begin())->getHandle());

			Picture *sprite_l = new Picture(_icons, 64, 64, {1}, {1});
			SpriteInfo const &info_l = _mapIcons.at(ent_l->_model._id);
			sprite_l->setState(info_l.state);
			sprite_l->setFrame(info_l.frame);

			SelectionPicture * selectionPicture_l = new SelectionPicture();
			selectionPicture_l->_icon = new SpriteModel();
			selectionPicture_l->_icon->sprite = sprite_l;
			selectionPicture_l->_icon->unitModel = dynamic_cast<octopus::UnitModel const *>(&ent_l->_model);
			selectionPicture_l->_icon->buildingModel = dynamic_cast<octopus::BuildingModel const *>(&ent_l->_model);

			selectionPicture_l->_text = new Text(&window_p, SDL_Color {0, 0, 0}, 0, 0);
			selectionPicture_l->_text->setText(std::to_string(set_l.size()));

			_selectionPictures.push_back(selectionPicture_l);
		}
	}

	// index used of position
	size_t idx_l = 0;
	_grid.clear();
	// update position of selection icons
	for(SelectionPicture * selectionPicture_l : _selectionPictures)
	{
		int x = idx_l % _iconsPerLine;
		int y = idx_l/_iconsPerLine;

		selectionPicture_l->_icon->sprite->setDestination(_x + 1 + x * 65, _y + 1 + y * 65, 64, 64);
		_grid[{x,y}] = selectionPicture_l->_icon;

		selectionPicture_l->_text->setPosition(_x + x * 65, _y + y * 65);

		++idx_l;
	}
}

void StatsPanel::render(Window &window_p)
{
	_background->display(window_p);
	for(SelectionPicture const * picture_l : _selectionPictures)
	{
		picture_l->_icon->sprite->display(window_p);
		picture_l->_text->display(window_p);
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

			if(_monoSelection->getQueue().hasCommand())
			{
				auto it_l = _monoSelection->getQueue().getCurrentCommand();
				size_t i = 0;
				while(it_l != _monoSelection->getQueue().getEnd() && i < _productionPictures.size())
				{
					octopus::BuildingUnitProductionCommand const *cmd_l = dynamic_cast<octopus::BuildingUnitProductionCommand const *>(it_l->_cmd);
					octopus::UnitProductionData const *data_l = dynamic_cast<octopus::UnitProductionData const *>(it_l->_data);
					if(cmd_l && data_l)
					{
						SpriteInfo const &info_l = _mapIcons.at(cmd_l->getModel()._id);
						_productionPictures[i]->_icon->setState(info_l.state);
						_productionPictures[i]->_icon->setFrame(info_l.frame);
						_productionPictures[i]->_icon->display(window_p);
						_productionPictures[i]->_bar->setProgress(100.*data_l->_progression/data_l->_completeTime);
						_productionPictures[i]->_bar->display(window_p);
					}
					++it_l;
					++i;
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
	for(SelectionPicture const * selectionPicture_l : _selectionPictures)
	{
		if(selectionPicture_l->_icon->sprite->isInside(window_p, x, y))
		{
			return selectionPicture_l->_icon;
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
