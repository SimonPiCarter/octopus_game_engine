#include "SelectionPicture.hh"

#include "panel/Panel.hh"
#include "text/Text.hh"

namespace cuttlefish
{

SelectionPicture::~SelectionPicture()
{
    delete _icon->sprite;
    delete _icon;
    delete _text;
}

} // namespace cuttlefish
