#ifndef __DialogStep__
#define __DialogStep__

#include "step/custom/CustomStep.hh"

#include "sprite/Picture.hh"

namespace cuttlefish
{
/// @brief this is a place holder step to allow step overrinding
class DialogStep : public octopus::CustomStep
{
public:
    DialogStep(std::string const &name_p, std::string const &text_p, Picture const &picture_p)
        : _name(name_p), _text(text_p), _picture(picture_p)
    {}
    virtual void apply(octopus::State &, octopus::SteppableData *) const {}
    virtual void revert(octopus::State &, octopus::SteppableData *) const {}
    virtual bool isNoOp() const { return false; }

    std::string const _name;
    std::string const _text;
    Picture const _picture;
};

}

#endif