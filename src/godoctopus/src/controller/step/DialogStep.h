#ifndef __Godoctopus_DialogStep__
#define __Godoctopus_DialogStep__

#include "step/custom/CustomStep.hh"

namespace godot
{
/// @brief this step triggers a dialog
class DialogStep : public octopus::CustomStep
{
public:
    DialogStep(std::string const &dialog_p)
        : _dialog(dialog_p)
    {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

    std::string const _dialog;
};

}

#endif
