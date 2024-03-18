#ifndef __DivinityGenerator__
#define __DivinityGenerator__

#include <functional>
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
    DivinityGenerator(
		unsigned long player_p,
		std::function<std::vector<fas::DivinityType>(octopus::State const &)> const &options_p
	) : _player(player_p), _optionsGenerator(options_p) {}

    virtual StepOptionsGenerator* newCopy() const override { return new DivinityGenerator(_player, _optionsGenerator); }

	void genOptions(octopus::State const &state_p) override;

    virtual std::vector<octopus::Steppable *> genSteppables(octopus::State const &state_p, unsigned long options_p) const override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    unsigned long const _player;
    std::vector<fas::DivinityType> _options;
	std::function<std::vector<fas::DivinityType>(octopus::State const &)> _optionsGenerator;
};

#endif
