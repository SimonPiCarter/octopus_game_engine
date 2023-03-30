#include "ChoicePanel.hh"

#include "step/player/PlayerAddOptionStep.hh"
#include "command/player/PlayerChoseOptionCommand.hh"

#include <iostream>

namespace fas
{

ChoicePanel::ChoicePanel(cuttlefish::Window &window_p, int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const *icons_p, unsigned long player_p)
    : cuttlefish::OptionPanel(x, y, background_p), _window(window_p), _player(player_p)
{
	_background->setDestination(x, y, 600., 300.);

	cuttlefish::Picture background_l(background_p, 400, 400, {1}, {1});

	background_l.setDestination(x+5, y+5, 190., 290.);
	_optionsSubPanel.push_back(new ChoiceSubPanel(window_p, x+5, y+5, 190, background_l, icons_p));

	background_l.setDestination(x+205, y+5, 190., 290.);
	_optionsSubPanel.push_back(new ChoiceSubPanel(window_p, x+205, y+5, 190, background_l, icons_p));

	background_l.setDestination(x+405, y+5, 190., 290.);
	_optionsSubPanel.push_back(new ChoiceSubPanel(window_p, x+405, y+5, 190, background_l, icons_p));
}

ChoicePanel::~ChoicePanel()
{
	for(cuttlefish::Picture * pic_l : _optionsBackground)
	{
		delete pic_l;
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

	if(_queuedOptions.size() == 1)
	{
		updateCurrent();
	}
}

/// @brief refresh Panel if necessary
void ChoicePanel::refresh()
{}

/// @brief display panel
void ChoicePanel::render(cuttlefish::Window &window_p)
{
	_background->display(window_p);
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

/// @brief return true if the panel as active options to be chosen
bool ChoicePanel::isActive() const
{
	return !_options.empty();
}

/// @brief returna a new command to be commited to the controller based on the option
/// @param option_p is supposed to be valid (>= 0)
octopus::Command * ChoicePanel::newCommandFromOption(int option_p)
{
	// pop first queue
	_queuedOptions.pop_front();
	_queuedKeys.pop_front();

	updateCurrent();

	return new octopus::PlayerChoseOptionCommand(_player, _key, option_p);
}

void ChoicePanel::updateCurrent()
{
	for(cuttlefish::SegmentedText * text_l : _optionsTexts)
	{
		delete text_l;
	}
	_optionsTexts.clear();

	/// @todo update visuals
	if(_queuedOptions.size() > 0)
	{
		_options = _queuedOptions.front();
		_key = _queuedKeys.front();
		size_t i = 0;
		/// temporary texts
		for(Option const &opt_l : _options)
		{
			std::visit([&](auto && arg) { _optionsSubPanel[i]->update(arg); }, opt_l);
			++i;
		}
	}
	else
	{
		_options.clear();
	}
}

} // namespace fas