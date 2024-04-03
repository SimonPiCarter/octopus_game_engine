#ifndef __BuffGenerators__
#define __BuffGenerators__

#include <functional>
#include <variant>
#include <vector>

#include "state/player/StepOptionsGenerator.hh"
#include "state/entity/buff/AnyBuff.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "utils/RandomGenerator.hh"

#include "library/model/divinity/DivinityModelLoader.hh"
#include "library/model/survival/SurvivalModelLoader.hh"

namespace octopus
{
    class State;
    class Library;
} // namespace octopus

struct NoOption {};

struct BuffOption
{
    unsigned long _player;
    octopus::AnyBuff _buff;
    std::string _model;
    std::string _div {""};
};

struct DoubleBuffOption
{
    unsigned long _player;
    octopus::AnyBuff _buff1;
    octopus::AnyBuff _buff2;
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

struct SurvivalOption
{
    unsigned long _player;
    fas::SurvivalSpecialType _type;
};

using SingleOption = std::variant<NoOption, BuffOption, ModifierOption, DoubleBuffOption, DivinityOption, SurvivalOption>;

struct Option
{
    SingleOption _playerOption = NoOption();
    SingleOption _enemyOption = NoOption();
};

class BuffGenerator : public octopus::StepOptionsGenerator
{
public:
    BuffGenerator(
		std::string const &key_p,
		std::function<std::vector<Option>(octopus::State const &)> const &options_p,
		octopus::Library const &lib_p) :
			octopus::StepOptionsGenerator(key_p),
			_optionsGenerator(options_p),
			_lib(lib_p)
		{}

	void genOptions(octopus::State const &state_p) override { _options = _optionsGenerator(state_p); }

    virtual StepOptionsGenerator* newCopy() const override { return new BuffGenerator(_key, _optionsGenerator, _lib); }

    virtual std::vector<octopus::Steppable *> genSteppables(octopus::State const &state_p, unsigned long options_p) const override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    std::vector<Option> _options;
	std::function<std::vector<Option>(octopus::State const &)> _optionsGenerator;

private:
    octopus::Library const &_lib;
};

SingleOption generatePlayerOption(octopus::State const &state_p, unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p, bool rare_p);
SingleOption generateEnemyOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p);

#endif
