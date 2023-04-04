#ifndef __DialogStep__
#define __DialogStep__

#include "step/custom/CustomStep.hh"

#include "sprite/Picture.hh"

namespace cuttlefish
{
/// @brief this step pops a dialog
class DialogStep : public octopus::CustomStep
{
public:
    DialogStep(std::string const &name_p, std::string const &text_p, Picture const &picture_p, std::string const &return_p="...")
        : _name(name_p), _text(text_p), _return(return_p), _picture(picture_p)
    {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

    std::string const _name;
    std::string const _text;
    std::string const _return;
    Picture const _picture;
};

}

#endif
