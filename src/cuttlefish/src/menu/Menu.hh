#ifndef __Menu__
#define __Menu__

#include <list>
#include <functional>

union SDL_Event;

namespace cuttlefish
{

class Picture;
class Text;
class Window;

/// @brief define a button as a combination of Picture + function
struct Button
{
    Picture * picture;
    Text * text;
    std::function<void()> callback;
};

/// @brief This class enable to set up menu composed of buttons
/// it allows to
/// - add buttons
/// - set spacing between buttons
/// - add a position to the menu
class Menu
{
public:
    Menu(int x, int y, int spacing_p);
    ~Menu();

    /// @brief add a button to the menu
    /// @param picture_p the picture representing the button; the ownership is taken
    /// @param text_p text to be displayed on top of the button; the ownership is taken
    /// @param callback_p the function to call on button clic
    /// @note button are supposed to have states in this order
    /// - idle
    /// - highlighted
    /// - pushed
    void addButton(Picture *picture_p, Text *text_p, std::function<void(void)> callback_p);

    void handleEvent(Window &window_p, SDL_Event const &e);

    void display(double elapsedTime_p, Window &window_p);

private:
    /// @brief list of buttons
    std::list<Button> _buttons;

    /// @brief a pointer to the clicked picture if any
    /// This is used to know if we should callback (callback is trigger iif mouse down and up happens on the same button)
    Picture *_clicked {nullptr};

    /// @brief spacing between buttons
    int const _spacing {0};

    int const _x {0};
    int const _y {0};
};

} // namespace cuttlefish


#endif