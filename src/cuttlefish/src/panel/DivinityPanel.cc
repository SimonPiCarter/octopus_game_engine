#include "DivinityPanel.hh"

#include "command/player/PlayerChoseDivinityCommand.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace cuttlefish
{

DivinityPanel::DivinityPanel(int x, int y, Texture const * background_p, Texture const *icons_p, unsigned long player_p) :
	OptionPanel(x, y, background_p), _player(player_p), _icons(icons_p)
{
}

DivinityPanel::~DivinityPanel()
{
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
		Picture *sprite_l = new Picture(_icons, 64, 64, {1}, {1});

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

		Picture *sprite_l = pair_l.first;
		sprite_l->setDestination(_x + x * 65, _y, 64, 64);
		_grid[{x,0}] = pair_l.second;
		++idx_l;
	}
}

void DivinityPanel::render(Window &window_p)
{
	_background->display(window_p);
	for(auto &&pair_l : _options)
	{
		pair_l.first->display(window_p);
	}
}

void DivinityPanel::addOptionInfo(octopus::DivinityType type_p, int state_p, int frame_p)
{
	_mapIcons[type_p].state = state_p;
	_mapIcons[type_p].frame = frame_p;
}

int DivinityPanel::getOption(Window &window_p, int x, int y) const
{
	int option_l = 0;
	for(auto &&pair_l : _options)
	{
		if(pair_l.first->isInside(window_p, x, y))
		{
			return option_l;
		}
		++option_l;
	}
	return -1;
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

void DivinityPanel::addOptionDivinityLayer(unsigned long player_p, std::set<octopus::DivinityType> const &options_p)
{
	if(_player == player_p)
	{
		_optionLayers.push_back(options_p);
	}
}

octopus::Command * DivinityPanel::newCommandFromOption(int option_p)
{
	int cur_l = 0;

	for(auto &&pair_l : _options)
	{
		if(cur_l == option_p)
		{
			octopus::PlayerChoseDivinityCommand * command_l = new octopus::PlayerChoseDivinityCommand(_player, pair_l.second);
			_optionLayers.pop_front();
			return command_l;
		}
		++cur_l;
	}

	return nullptr;
}

} // namespace cuttlefish
