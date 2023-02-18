#include "DescPanel.hh"

#include "texture/Texture.hh"
#include "sprite/Sprite.hh"

#include "state/State.hh"
#include "state/model/entity/EntityModel.hh"

namespace cuttlefish
{

DescPanel::DescPanel(Window* window_p, int x, int y, Texture const * background_p) :
	_x(x), _y(y), _desc(window_p, x+2, y+2, 256)
{
	_background = new Sprite(0, background_p, 260./64., 0, 0, 400, 400, {1}, {1}, true);
	_background->setPosition(x, y);
}

DescPanel::~DescPanel()
{
	delete _background;
}

void DescPanel::setText(std::string const &text_p)
{
	_desc.setText(text_p, {0, 0, 0});
}

void DescPanel::render(Window &window_p)
{
	_background->render(window_p);
	_desc.display(window_p);
}

std::string getDesc(octopus::EntityModel const &model_p)
{
	std::stringstream ss_l;
	ss_l<<model_p._id<<" $$ ";
	for(auto &&pair_l : model_p._cost)
	{
		if(pair_l.second > 0)
		{
			ss_l<<to_string(pair_l.first)<<" : "<<pair_l.second<<" $$ ";
		}
	}
	return ss_l.str();
}

} // namespace cuttlefish
