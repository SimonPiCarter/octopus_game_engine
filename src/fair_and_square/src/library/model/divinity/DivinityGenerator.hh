#ifndef __DivinityGenerator__
#define __DivinityGenerator__

#include <vector>

#include "state/player/StepOptionsGenerator.hh"
#include "DivinityModelLoader.hh"

namespace octopus
{
    class State;
} // namespace octopus

class DivinityGenerator : public octopus::StepOptionsGenerator
{
public:
    DivinityGenerator(unsigned long player_p, std::vector<fas::DivinityType> const &options_p) : _player(player_p), _options(options_p) {}

    virtual StepOptionsGenerator* newCopy() const override { return new DivinityGenerator(_player, _options); }

    virtual std::vector<octopus::Steppable *> getSteppables(unsigned long options_p) const override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    unsigned long const _player;
    std::vector<fas::DivinityType> const _options;
};

#endif
