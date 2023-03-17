#include "ChoicePanel.hh"

#include "library/model/bonus/buffGenerators.hh"
#include "step/player/PlayerAddOptionStep.hh"

#include <iostream>

namespace fas
{

ChoicePanel::ChoicePanel(int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const *icons_p, unsigned long player_p)
    : cuttlefish::DivinityPanel(x, y, background_p, icons_p, player_p)
{}

void ChoicePanel::addOptionLayer(octopus::PlayerAddOptionStep const *options_p)
{
	BuffGenerator const *gen_l = dynamic_cast<BuffGenerator const *>(options_p->_generator);

	std::cout<<"options found"<<std::endl;
	for(BuffOption const &opt_l : gen_l->_options)
	{
		std::cout<<"option : "<<std::endl;
		std::cout<<"\ttype : "<<octopus::to_string(opt_l._buff._type)<<std::endl;
		std::cout<<"\toffset : "<<opt_l._buff._offset<<std::endl;
		std::cout<<"\tcoef : "<<opt_l._buff._coef<<std::endl;
	}
}

} // namespace fas