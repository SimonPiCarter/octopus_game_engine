#ifndef __SelectionPicture__
#define __SelectionPicture__

namespace cuttlefish
{
class SpriteModel;
class Text;

/// @brief This class handles basic icon picture
/// plus a text for selection
/// - display unit
/// - number selected
class SelectionPicture
{
public:
    ~SelectionPicture();

    SpriteModel * _icon {nullptr};
    Text * _text {nullptr};
};

} // namespace cuttlefish


#endif
