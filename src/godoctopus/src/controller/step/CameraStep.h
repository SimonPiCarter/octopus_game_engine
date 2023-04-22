#ifndef __Godoctopus_CameraStep__
#define __Godoctopus_CameraStep__

#include "step/custom/CustomStep.hh"

namespace godot
{
/// @brief this step allow to move camera
class CameraStep : public octopus::CustomStep
{
public:
    CameraStep(int x, int y)
        : _x(x), _y(y)
    {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

    int const _x;
    int const _y;
};

}

#endif
