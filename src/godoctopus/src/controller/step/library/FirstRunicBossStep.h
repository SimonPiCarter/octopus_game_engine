#ifndef __Godoctopus_FirstRunicBossStep__
#define __Godoctopus_FirstRunicBossStep__

#include "step/custom/CustomStep.hh"

namespace godot
{

/// @brief this step trigger aoe info for first runic boss
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

class FirstRunicBossPillar : public octopus::CustomStep
{
public:
	FirstRunicBossPillar(int id_p, bool spawn_p, bool firstSpawn_p) : _id(id_p), _spawn(spawn_p), _firstSpawn(firstSpawn_p) {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

	int const _id;
	bool const _spawn;
	bool const _firstSpawn;
};

}

#endif
