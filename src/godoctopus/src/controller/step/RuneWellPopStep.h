#ifndef __Godoctopus_RuneWellPopStep__
#define __Godoctopus_RuneWellPopStep__

#include "step/custom/CustomStep.hh"

namespace godot
{
/// @brief this step will trigger a rune well animation
class RuneWellPopStep : public octopus::CustomStep
{
public:
    RuneWellPopStep(unsigned long wellIdx_p, unsigned long player_p) : _wellIdx(wellIdx_p), _player(player_p) {}
    virtual void apply(octopus::State &) const {}
    virtual void revert(octopus::State &, octopus::SteppableData const *) const {}
    virtual bool isNoOp() const { return false; }

	unsigned long const _wellIdx;
	unsigned long const _player;
};

}

#endif
