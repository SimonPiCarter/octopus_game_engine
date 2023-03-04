#ifndef __ProductionPicture__
#define __ProductionPicture__

namespace cuttlefish
{

class ProgressBar;
class Picture;

/// @brief This class handles basic icon picture
/// plus a progress bar on top to track production
class ProductionPicture
{
public:
    ~ProductionPicture();

    Picture *_icon {nullptr};
    ProgressBar *_bar {nullptr};
};

} // namespace cuttlefish


#endif
