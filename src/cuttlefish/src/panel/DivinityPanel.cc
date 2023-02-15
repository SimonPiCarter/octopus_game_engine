#include "DivinityPanel.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace cuttlefish
{

DivinityPanel::DivinityPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, unsigned long player_p) :
	_x(x), _y(y), _player(player_p), _icons(icons_p)
{
	_background = new Sprite(0, background_p, 200./64., 0, 0, 400, 400, {1}, {1}, true);
	_background->setPosition(x, y);
}

DivinityPanel::~DivinityPanel()
{
	delete _background;
	cleanOptions();
}

void DivinityPanel::refresh()
{
	std::set<octopus::DivinityType> curDivTypes_l;
	for(auto &&pair_l : _options)
	{
		curDivTypes_l.insert(pair_l.second);
	}

	std::set<octopus::DivinityType> newDivTypes_l;
	if(!_optionLayers.empty())
	{
		newDivTypes_l = _optionLayers.front();
	}

	if(curDivTypes_l == newDivTypes_l)
	{
		return;
	}

	cleanOptions();

	for(octopus::DivinityType type_l : newDivTypes_l)
	{
		Sprite *sprite_l = new Sprite(0, _icons, 1, 0, 0, 64, 64, {1}, {1}, true);

		OptionInfo const &info_l = _mapIcons.at(type_l);
		sprite_l->setState(info_l.state);
		sprite_l->setFrame(info_l.frame);

		_options.push_back({sprite_l, type_l});
	}

	// index used of position
	size_t idx_l = 0;
	// update position of sprites
	for(auto &&pair_l : _options)
	{
		int x = idx_l;

		Sprite *sprite_l = pair_l.first;
		sprite_l->setPosition(_x + x * 65, _y);
		_grid[{x,0}] = pair_l.second;
		++idx_l;
	}
}

void DivinityPanel::render(Window &window_p)
{
	_background->render(window_p);
	for(auto &&pair_l : _options)
	{
		pair_l.first->render(window_p);
	}
}

void DivinityPanel::addOptionInfo(octopus::DivinityType type_p, int state_p, int frame_p)
{
	_mapIcons[type_p].state = state_p;
	_mapIcons[type_p].frame = frame_p;
}

std::pair<bool, octopus::DivinityType> DivinityPanel::getOption(Window &window_p, int x, int y) const
{
	for(auto &&pair_l : _options)
	{
		if(pair_l.first->isInside(window_p, x, y))
		{
			return {true, pair_l.second};
		}
	}
	return {false, octopus::DivinityType::Divinity_1};
}

std::pair<bool, octopus::DivinityType> DivinityPanel::getOptionOnGrid(int x, int y) const
{
	auto it_l = _grid.find({x,y});
	if(it_l == _grid.end())
	{
		return {false, octopus::DivinityType::Divinity_1};
	}
	return {true, it_l->second};
}

void DivinityPanel::cleanOptions()
{
	// delete sprites
	for(auto &&pair_l : _options)
	{
		delete pair_l.first;
	}
	_options.clear();
	_grid.clear();
}

void DivinityPanel::addOptionLayer(unsigned long player_p, std::set<octopus::DivinityType> const &options_p)
{
	if(_player == player_p)
	{
		_optionLayers.push_back(options_p);
	}
}

void DivinityPanel::popOptionLayer()
{
	_optionLayers.pop_front();
}

} // namespace cuttlefish
