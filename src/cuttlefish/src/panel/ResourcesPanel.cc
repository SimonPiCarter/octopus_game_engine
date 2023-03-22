#include "ResourcesPanel.hh"

#include "state/State.hh"
#include "state/player/Player.hh"

namespace cuttlefish
{

ResourcesPanel::ResourcesPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const * resIcons_p) :
	_x(x), _y(y),
	_res1Icon(resIcons_p, 32, 32, {1}, {1}),
	_res2Icon(resIcons_p, 32, 32, {1}, {1}),
	_timeIcon(resIcons_p, 32, 32, {1}, {1}),
	_textRes1(window_p, x+37, y+8),
	_textRes2(window_p, x+37, y+40),
	_textIcon(window_p, x+37, y+72)
{
	_background = new Picture(background_p, 400, 400, {1}, {1});
	_background->setDestination(x, y, 100., 100.);

	_res1Icon.setDestination(x+5, y + 5, 32, 32);
	_res2Icon.setDestination(x+5, y + 37, 32, 32);
	_timeIcon.setDestination(x+5, y + 69, 32, 32);

	_textRes1.addText("val", "", {0, 0, 0}, false);
	_textRes2.addText("val", "", {0, 0, 0}, false);
	_textIcon.addText("val", "", {0, 0, 0}, false);
}

ResourcesPanel::~ResourcesPanel()
{
	delete _background;
}

std::string getVal(octopus::Player const &player_p, octopus::ResourceType type_p)
{
	return stringify(std::abs(std::floor(1e-5+octopus::getResource(player_p, type_p))));
}

void ResourcesPanel::refresh(Window &window_p, octopus::State const &state_p, unsigned long player_p)
{
	octopus::Player const &player_l = *state_p.getPlayer(player_p);
	_textRes1.updateText("val", getVal(player_l, octopus::ResourceType::Food));
	_textRes2.updateText("val", getVal(player_l, octopus::ResourceType::Steel));
	_textIcon.updateText("val", getVal(player_l, octopus::ResourceType::Anchor));
}

void ResourcesPanel::render(Window &window_p)
{
	_background->display(window_p);
	_res1Icon.display(window_p);
	_res2Icon.display(window_p);
	_timeIcon.display(window_p);
	_textRes1.display(window_p);
	_textRes2.display(window_p);
	_textIcon.display(window_p);
}
void ResourcesPanel::setRes1Icon(int state_p, int frame_p)
{
	_res1Icon.setState(state_p);
	_res1Icon.setFrame(frame_p);
}
void ResourcesPanel::setRes2Icon(int state_p, int frame_p)
{
	_res2Icon.setState(state_p);
	_res2Icon.setFrame(frame_p);
}
void ResourcesPanel::setTimeIcon(int state_p, int frame_p)
{
	_timeIcon.setState(state_p);
	_timeIcon.setFrame(frame_p);
}


} // namespace cuttlefish
