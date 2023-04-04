#ifndef __CameraStep__
#define __CameraStep__

#include "step/custom/CustomStep.hh"

#include "sprite/Picture.hh"

namespace cuttlefish
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
