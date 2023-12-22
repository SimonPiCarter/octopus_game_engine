#ifndef __Godoctopus_WaveStep__
#define __Godoctopus_WaveStep__

#include "step/custom/CustomStep.hh"

namespace godot
{

/// @brief this step triggers a wave on the UI
class WaveStep : public octopus::CustomStep
{
public:
    WaveStep() {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }
};

/// @brief this step triggers spawn point for the incoming wave
class WaveSpawPointStep : public octopus::CustomStep
{
public:
    WaveSpawPointStep(std::vector<octopus::Vector> const &points_p) : _points(points_p) {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

	std::vector<octopus::Vector> const &getPoints() const { return _points; }
protected:
	std::vector<octopus::Vector> _points;
};

}

#endif
