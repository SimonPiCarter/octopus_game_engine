#ifndef __ProgressBar__
#define __ProgressBar__

#include "sprite/Picture.hh"

namespace cuttlefish
{

/// @brief this class is aimed at representing a progress bar
/// it is composed of two Picture
/// - One for the background
/// - Another for the progress marker
class ProgressBar
{
public:
    /// @brief create a progress bar
    /// @param background_p the picture that will be used as background
    /// @param bar_p the picture that will be used as the bar to show progress
    /// @param w width of the background
    /// @param h height of the background
    /// @param margin_p margin between background and the bar (in pixel)
    /// @note the pictures are owend by this ProgressBar
    ProgressBar(Picture *background_p, Picture *bar_p, int w, int h, int margin_p);
    ~ProgressBar();

    /// @brief update pictures based on elapsed time and refresh rate
    void update(double elapsedTime_p);

    /// @brief render pictures
    void display( Window &window_p);

    /// @brief set progress value will be clamped between 0 and 100
    void setProgress(int progress_p);

    /// @brief set bar position
    void setPosition(int x, int y);

    int getWidth() const { return _w; }
    int getHeight() const { return _h; }

private:
    /// @brief The picture of the background
    Picture * const _background;
    /// @brief The picture of the bar
    Picture * const _bar;
    /// @brief size on screen of the background
    int const _w;
    /// @brief size on screen of the background
    int const _h;
    /// @brief the margin in pixels between thee background and the progress bar
    int const _margin;
    /// @brief max width the progressing part can be on screen
    int const _maxW;

    /// @brief current progress
    int _progress {0};
    /// @brief current width on screen
    int _curW {0};
};

} // namespace cuttlefish


#endif
