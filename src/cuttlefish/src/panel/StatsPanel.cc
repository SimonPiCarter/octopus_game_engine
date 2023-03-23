#include "StatsPanel.hh"

#include "window/Window.hh"

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

StatsPanel::StatsPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, Texture const *statsIcons_p, int iconsPerLine_p, Selection &selection_p) :
	_x(x),
	_y(y),
	_background(new Picture(background_p, 276, 276, {1}, {1})),
	_icons(icons_p),
	_iconsPerLine(iconsPerLine_p),
	_textResources(window_p, x, y+45),
	_textQtyRes(window_p, x+150, y+45),
	_selection(selection_p),
	_monoIcon(icons_p, 64, 64, {1}, {1}),
	_damageIcon(statsIcons_p, 32, 32, {1}, {1}),
	_armorIcon(statsIcons_p, 32, 32, {1}, {1}),
	_attackSpeedIcon(statsIcons_p, 32, 32, {1}, {1}),
	_speedIcon(statsIcons_p, 32, 32, {1}, {1}),
	_textHp(window_p, x+75, y+45),
	_textDamage(window_p, x+50, y+95),
	_textArmor(window_p, x+50, y+122),
	_textAttackSpeed(window_p, x+50, y+152),
	_textSpeed(window_p, x+50, y+182),
	_hpBar(new Picture(window_p->loadTexture("resources/fair_and_square/hp_bar_back.png"), 32, 6, {1}, {1}),
			new Picture(window_p->loadTexture("resources/fair_and_square/hp_bar_fill.png"), 32, 4, {1}, {1}), 266, 6, 1)
{
	_background->setDestination(x, y, 276, 276);
	_hpBar.setPosition(x+5, y+70);

	_monoIcon.setDestination(x+5, y+5, 64, 64);
	_damageIcon.setDestination(x+5, y+90, 32, 32);
	_damageIcon.setState(1);
	_damageIcon.setFrame(0);
	_armorIcon.setDestination(x+5, y+120, 32, 32);
	_armorIcon.setState(2);
	_armorIcon.setFrame(0);
	_attackSpeedIcon.setDestination(x+5, y+150, 32, 32);
	_attackSpeedIcon.setState(3);
	_attackSpeedIcon.setFrame(0);
	_speedIcon.setDestination(x+5, y+180, 32, 32);
	_speedIcon.setState(2);
	_speedIcon.setFrame(1);

	_textHp.addText("hp_val", "", {0, 0, 0}, false);
	_textDamage.addText("dmg_val", "", {0, 0, 0}, false);
	_textArmor.addText("armor_val", "", {0, 0, 0}, false);
	_textAttackSpeed.addText("attackSpeed_val", "", {0, 0, 0}, false);
	_textSpeed.addText("speed_val", "", {0, 0, 0}, false);

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
}

void StatsPanel::refresh(Window &window_p, octopus::State const &state_p)
{
	// nothing to do (when muliple selection)
	SelectionKey curKey_l = _selection.key();
	if(_lastKey == curKey_l && _selection._sprites.size() > 1)
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
		_monoSelection = nullptr;
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

		selectionPicture_l->_icon->sprite->setDestination(_x + 4 + x * 68, _y + 4 + y * 68, 64, 64);
		_grid[{x,y}] = selectionPicture_l->_icon;

		selectionPicture_l->_text->setPosition(_x + 4 + x * 68, _y + 4 + y * 68);

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
			SpriteInfo const &info_l = _mapIcons.at(_monoSelection->_model._id);
			_monoIcon.setState(info_l.state);
			_monoIcon.setFrame(info_l.frame);

			std::stringstream ss_l;
			ss_l<<_monoSelection->_hp<<"/"<<_monoSelection->getHpMax();
			_textHp.updateText("hp_val", ss_l.str());

			ss_l.str("");
			ss_l<<_monoSelection->getDamageNoBonus();
			_textDamage.updateText("dmg_val", ss_l.str());

			ss_l.str("");
			ss_l<<_monoSelection->getArmor();
			_textArmor.updateText("armor_val", ss_l.str());

			ss_l.str("");
			ss_l<<_monoSelection->getStepSpeed()*100;
			_textSpeed.updateText("speed_val", ss_l.str());

			ss_l.str("");
			ss_l<<_monoSelection->getFullReload()/100.;
			_textAttackSpeed.updateText("attackSpeed_val", ss_l.str());

			// display stats on selection
			_textHp.display(window_p);
			_textDamage.display(window_p);
			_textArmor.display(window_p);
			_textAttackSpeed.display(window_p);
			_textSpeed.display(window_p);

			_monoIcon.display(window_p);
			_damageIcon.display(window_p);
			_armorIcon.display(window_p);
			_attackSpeedIcon.display(window_p);
			_speedIcon.display(window_p);

			_hpBar.setProgress(100*_monoSelection->_hp/_monoSelection->getHpMax());
			_hpBar.display(window_p);

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
