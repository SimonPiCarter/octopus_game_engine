#include "ChoicePanel.hh"

#include "step/player/PlayerAddOptionStep.hh"
#include "command/player/PlayerChoseOptionCommand.hh"

#include <iostream>

namespace fas
{

ChoicePanel::ChoicePanel(int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const *icons_p, unsigned long player_p)
    : cuttlefish::OptionPanel(x, y, background_p), _player(player_p)
{
	_background->setDestination(x, y, 600., 300.);

	_optionsBackground.push_back(new cuttlefish::Picture(background_p, 400, 400, {1}, {1}));
	_optionsBackground.back()->setDestination(x+5, y+5, 190., 290.);
	_optionsBackground.push_back(new cuttlefish::Picture(background_p, 400, 400, {1}, {1}));
	_optionsBackground.back()->setDestination(x+205, y+5, 190., 290.);
	_optionsBackground.push_back(new cuttlefish::Picture(background_p, 400, 400, {1}, {1}));
	_optionsBackground.back()->setDestination(x+405, y+5, 190., 290.);
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

	std::cout<<"options found"<<std::endl;
	_options = gen_l->_options;

	_key = options_p->_key;

	for(BuffOption const &opt_l : _options)
	{
		std::cout<<"option : "<<std::endl;
		std::cout<<"\ttype : "<<octopus::to_string(opt_l._buff._type)<<std::endl;
		std::cout<<"\toffset : "<<opt_l._buff._offset<<std::endl;
		std::cout<<"\tcoef : "<<opt_l._buff._coef<<std::endl;
		std::cout<<"\tmodel : "<<opt_l._model<<std::endl;
	}
}

/// @brief refresh Panel if necessary
void ChoicePanel::refresh()
{}

/// @brief display panel
void ChoicePanel::render(cuttlefish::Window &window_p)
{
	_background->display(window_p);
	for(cuttlefish::Picture * pic_l : _optionsBackground)
	{
		pic_l->display(window_p);
	}
}

/// @brief return the option clicked
/// @note return -1 if no option was picked
int ChoicePanel::getOption(cuttlefish::Window &window_p, int x, int y) const
{
	int count_l = 0;
	for(cuttlefish::Picture * pic_l : _optionsBackground)
	{
		if(pic_l->isInside(window_p, x, y))
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
	_options.clear();
	return new octopus::PlayerChoseOptionCommand(_player, _key, option_p);
}

} // namespace fas