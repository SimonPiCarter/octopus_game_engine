#ifndef __BuffGenerators__
#define __BuffGenerators__

#include <variant>
#include <vector>

#include "state/player/StepOptionsGenerator.hh"
#include "state/entity/Buff.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "utils/RandomGenerator.hh"

struct BuffOption
{
    unsigned long _player;
    octopus::TyppedBuff _buff;
    std::string _model;
};

struct ModifierOption
{
    unsigned long _player;
    octopus::AttackModifier _mod;
    std::string _model;
};

using SingleOption = std::variant<BuffOption, ModifierOption>;

struct Option
{
    SingleOption _playerOption;
    SingleOption _enemyOption;
};

class BuffGenerator : public octopus::StepOptionsGenerator
{
public:
    BuffGenerator(std::vector<Option> const &options_p) : _options(options_p) {}

    virtual StepOptionsGenerator* newCopy() const override { return new BuffGenerator(_options); }

    virtual std::vector<octopus::Steppable *> getSteppables(unsigned long options_p) const override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    std::vector<Option> const _options;
};

SingleOption generatePlayerOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p);
SingleOption generateEnemyOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p);

#endif
