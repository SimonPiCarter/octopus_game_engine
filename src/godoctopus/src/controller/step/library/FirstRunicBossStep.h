#ifndef __Godoctopus_FirstRunicBossStep__
#define __Godoctopus_FirstRunicBossStep__

#include "step/custom/CustomStep.hh"

namespace godot
{
/// @brief this step allow to move camera
class FirstRunicBossStep : public octopus::CustomStep
{
public:
    FirstRunicBossStep(int id_p, bool trigger_p, double x, double y, double range_p)
        : _id(id_p), _trigger(trigger_p), _x(x), _y(y), _range(range_p)
    {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

	int const _id;
	bool const _trigger;
    double const _x;
    double const _y;
	double const _range;
};

}

#endif
