#ifndef __Godoctopus_WaveStep__
#define __Godoctopus_WaveStep__

#include "step/custom/CustomStep.hh"

namespace godot
{
/// @brief this step triggers a dialog
class WaveStep : public octopus::CustomStep
{
public:
    WaveStep() {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }
};

}

#endif
