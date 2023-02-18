#ifndef __DescPanel__
#define __DescPanel__

#include "text/WrappedText.hh"

namespace octopus
{
	struct EntityModel;
}

namespace cuttlefish
{
class Sprite;
class Texture;
class Window;

/// @brief this class represent a DescPanel
/// displaying the options for the building
/// or unit
/// Display a background and an image for each model
class DescPanel
{
public:
	DescPanel(Window* window_p, int x, int y, Texture const * background_p);
	~DescPanel();

	/// @brief display descpanel
	void render(Window &window_p);

	void setText(std::string const &text_p);

	Sprite const * getBackground() const { return _background; }
protected:
	Sprite * _background;

	int _x;
	int _y;

	WrappedText _desc;
};

std::string getDesc(octopus::EntityModel const &model_p);
} // namespace octopus


#endif
