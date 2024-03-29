#ifndef __BuffGenerators__
#define __BuffGenerators__

#include <variant>
#include <vector>

#include "state/player/StepOptionsGenerator.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "utils/RandomGenerator.hh"

#include "library/model/divinity/DivinityModelLoader.hh"

namespace octopus
{
    class State;
    class Library;
} // namespace octopus


struct BuffOption
{
    unsigned long _player;
    octopus::TimedBuff _buff;
    std::string _model;
    std::string _div {""};
};

struct DoubleBuffOption
{
    unsigned long _player;
    octopus::TimedBuff _buff1;
    octopus::TimedBuff _buff2;
    std::string _model;
};

struct ModifierOption
{
    unsigned long _player;
    octopus::AttackModifier _mod;
    std::string _model;
};

struct DivinityOption
{
    unsigned long _player;
    fas::DivinityType _div;
};

using SingleOption = std::variant<BuffOption, ModifierOption, DoubleBuffOption, DivinityOption>;

struct Option
{
    SingleOption _playerOption;
    SingleOption _enemyOption;
};

class BuffGenerator : public octopus::StepOptionsGenerator
{
public:
    BuffGenerator(std::vector<Option> const &options_p, octopus::Library const &lib_p) : _options(options_p), _lib(lib_p) {}

    virtual StepOptionsGenerator* newCopy() const override { return new BuffGenerator(_options, _lib); }

    virtual std::vector<octopus::Steppable *> getSteppables(unsigned long options_p) const override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    std::vector<Option> const _options;

private:
    octopus::Library const &_lib;
};

SingleOption generatePlayerOption(octopus::State const &state_p, unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p, bool rare_p);
SingleOption generateEnemyOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p);

#endif
