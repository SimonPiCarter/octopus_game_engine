#ifndef __Dialog__
#define __Dialog__

#include "text/Text.hh"
#include "text/WrappedText.hh"
#include "sprite/Picture.hh"

namespace cuttlefish
{
class Window;

/// @brief This class allow to display dialog on screen
/// it is composed of multiple parts
/// - name of the speaking character
/// - one part for the text (a wrapped text)
/// - icon of the speaking character
class Dialog
{
public:
	Dialog(Window* window_p, int x, int y, Picture const & background_p,
        std::string const &name_p, std::string const &text_p, Picture const & icon_p, std::string const &return_p);
	~Dialog();

	/// @brief display Dialog
	void display(Window &window_p);

	Picture const &getBackground() const { return _background; }
private:
	Picture _background;

    Text _textName;
    WrappedText _textDescription;
    Picture _icon;
    Text _textReturn;
};
} // namespace cuttlefish


#endif
