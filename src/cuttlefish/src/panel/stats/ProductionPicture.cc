#include "ProductionPicture.hh"

#include "sprite/Picture.hh"
#include "progressBar/ProgressBar.hh"

namespace cuttlefish
{

ProductionPicture::~ProductionPicture()
{
    delete _icon;
    delete _bar;
}

} // namespace cuttlefish
