#ifndef __BuffGenerators__
#define __BuffGenerators__

#include <random>
#include <variant>
#include <vector>

#include "state/player/StepOptionsGenerator.hh"
#include "state/entity/Buff.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

struct BuffOption
{
    octopus::TyppedBuff _buff;
    std::string _model;
};

struct ModifierOption
{
    octopus::AttackModifier _mod;
    std::string _model;
};

using Option = std::variant<BuffOption, ModifierOption>;

class BuffGenerator : public octopus::StepOptionsGenerator
{
public:
    BuffGenerator(std::vector<Option> const &options_p) : _options(options_p) {}

    virtual StepOptionsGenerator* newCopy() const override { return new BuffGenerator(_options); }

    virtual std::vector<octopus::Steppable *> getSteppables(unsigned long options_p, unsigned long player_p) const override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    std::vector<Option> const _options;
};

BuffOption generateRandomBuffOption(std::mt19937 &_gen, std::string const &id_p);

ModifierOption generateRandomModifierOption(std::mt19937 &_gen);

#endif
