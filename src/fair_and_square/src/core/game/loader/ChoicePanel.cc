#include "ChoicePanel.hh"

#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"
#include "command/player/PlayerChoseOptionCommand.hh"

#include <iostream>

namespace fas
{

ChoicePanel::ChoicePanel(cuttlefish::Window &window_p, int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const * subBackground_p,
	cuttlefish::Texture const *icons_p, cuttlefish::Texture const *statsIcons_p, unsigned long player_p)
    : cuttlefish::OptionPanel(x, y, background_p), _window(window_p), _player(player_p),
		_subBackground(subBackground_p, 190, 350, {2}, {1}), _icons(icons_p), _statsIcons(statsIcons_p)
{
	_background->setDestination(x-300, y, 600., 360.);
}

ChoicePanel::~ChoicePanel()
{
	for(cuttlefish::Picture * pic_l : _optionsBackground)
	{
		delete pic_l;
	}
	for(ChoiceSubPanel *ptr_l : _optionsSubPanel)
	{
		delete ptr_l;
	}
}

void ChoicePanel::addOptionLayer(octopus::PlayerAddOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	BuffGenerator const *gen_l = dynamic_cast<BuffGenerator const *>(options_p->_generator);
	_queuedOptions.push_back(gen_l->_options);
	_queuedKeys.push_back(options_p->_key);

	_active = true;

	if(_queuedOptions.size() == 1)
	{
		updateCurrent();
	}
}

void ChoicePanel::popOptionLayer(octopus::PlayerPopOptionStep const *options_p)
{
	if(_chosenMode)
	{
		return;
	}
	// store chosen option to display them later
	_chosenOptions.push_back(_queuedOptions.front().at(options_p->_choice));

	// pop first queue
	_queuedOptions.pop_front();
	_queuedKeys.pop_front();

	updateCurrent();
}

/// @brief refresh Panel if necessary
void ChoicePanel::refresh()
{}

/// @brief display panel
void ChoicePanel::render(cuttlefish::Window &window_p)
{
	//_background->display(window_p);
	for(ChoiceSubPanel * subPanel_l : _optionsSubPanel)
	{
		subPanel_l->display(window_p);
	}
}

/// @brief return the option clicked
/// @note return -1 if no option was picked
int ChoicePanel::getOption(cuttlefish::Window &window_p, int x, int y) const
{
	int count_l = 0;
	for(ChoiceSubPanel const * subPanel_l : _optionsSubPanel)
	{
		if(subPanel_l->getBackground().isInside(window_p, x, y))
		{
			return count_l;
		}
		++count_l;
	}
	return -1;
}

void ChoicePanel::refreshFromMouse(cuttlefish::Window &window_p, int x, int y)
{
	for(ChoiceSubPanel * subPanel_l : _optionsSubPanel)
	{
		if(subPanel_l->getBackground().isInside(window_p, x, y))
		{
			subPanel_l->getBackground().setFrame(1);
		}
		else
		{
			subPanel_l->getBackground().setFrame(0);
		}
	}
}

/// @brief return true if the panel as active options to be chosen
bool ChoicePanel::isActive() const
{
	return _active;
}

/// @brief returna a new command to be commited to the controller based on the option
/// @param option_p is supposed to be valid (>= 0)
octopus::Command * ChoicePanel::newCommandFromOption(int option_p)
{
	if(_chosenMode)
	{
		return nullptr;
	}
	_active = false;

	return new octopus::PlayerChoseOptionCommand(_player, _key, option_p);
}

/// @brief load chosen option for display
void ChoicePanel::loadChosenOption()
{
	if(_chosenMode)
	{
		return;
	}
	_chosenMode = true;
	for(cuttlefish::SegmentedText * text_l : _optionsTexts)
	{
		delete text_l;
	}
	_optionsTexts.clear();
	for(ChoiceSubPanel *ptr_l : _optionsSubPanel)
	{
		delete ptr_l;
	}
	_optionsSubPanel.clear();

	if(_chosenOptions.size() > 0)
	{
		_options = _chosenOptions;
		_key = "";
		size_t i = 0;
		/// temporary texts
		for(Option const &opt_l : _options)
		{
			_subBackground.setDestination(_x+5+200*i, _y+5, 190., 350.);
			_optionsSubPanel.push_back(new ChoiceSubPanel(_window, _x+5+200*i, _y+5, 190, _subBackground, _icons, _statsIcons));

			_optionsSubPanel.back()->update(opt_l);
			++i;
		}
	}
	else
	{
		_options.clear();
	}
	_active = !_options.empty();
}

/// @brief unload chosen option for display
void ChoicePanel::unloadChosenOption()
{
	_chosenMode = false;
	updateCurrent();
}

void ChoicePanel::updateCurrent()
{
	for(cuttlefish::SegmentedText * text_l : _optionsTexts)
	{
		delete text_l;
	}
	_optionsTexts.clear();
	for(ChoiceSubPanel *ptr_l : _optionsSubPanel)
	{
		delete ptr_l;
	}
	_optionsSubPanel.clear();

	if(_queuedOptions.size() > 0)
	{
		_options = _queuedOptions.front();
		_key = _queuedKeys.front();
		size_t i = 0;
		/// temporary texts
		for(Option const &opt_l : _options)
		{
			_subBackground.setDestination(_x+5+200*i, _y+5, 190., 350.);
			_optionsSubPanel.push_back(new ChoiceSubPanel(_window, _x+5+200*i, _y+5, 190, _subBackground, _icons, _statsIcons));

			_optionsSubPanel.back()->update(opt_l);
			++i;
		}
	}
	else
	{
		_options.clear();
	}
	_active = !_options.empty();
}

} // namespace fas
