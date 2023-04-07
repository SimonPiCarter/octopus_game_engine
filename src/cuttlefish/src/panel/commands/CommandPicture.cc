#include "CommandPicture.hh"

#include "panel/Panel.hh"
#include "text/Text.hh"

namespace cuttlefish
{

CommandPicture::~CommandPicture()
{
    if(_model)
    {
        delete _model->sprite;
    }
	delete _model;
	delete _text;
}

} // namespace cuttlefish
