#ifndef __ResourcesPanel__
#define __ResourcesPanel__

#include "text/SegmentedText.hh"
#include "sprite/Picture.hh"

namespace octopus
{
	class State;
}

namespace cuttlefish
{
class Picture;
class Texture;
class Window;

/// @brief this class represent a ResourcesPanel
/// displaying the options for the building
/// or unit
/// Display a background and an image for each model
class ResourcesPanel
{
public:
	ResourcesPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const * resIcons_p);
	~ResourcesPanel();

	/// @brief refresh res panel
	void refresh(Window &window_p, octopus::State const &state_p, unsigned long player_p);

	/// @brief display res panel
	void render(Window &window_p);

	/// @brief set the state/frame to be used for resource 1
	void setRes1Icon(int state_p, int frame_p);
	/// @brief set the state/frame to be used for resource 2
	void setRes2Icon(int state_p, int frame_p);
	/// @brief set the state/frame to be used for time
	void setTimeIcon(int state_p, int frame_p);

	Picture const * getBackground() const { return _background; }
protected:
	Picture * _background;

	int _x;
	int _y;

	///
	/// Resources info
	///
	Picture _res1Icon;
	Picture _res2Icon;
	Picture _timeIcon;

	SegmentedText _textRes1;
	SegmentedText _textRes2;
	SegmentedText _textIcon;
};

} // namespace cuttlefish


#endif
