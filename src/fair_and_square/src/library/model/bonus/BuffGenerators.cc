#include "BuffGenerators.hh"

#include "state/entity/attackModifier/AttackModifier.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerAttackModAllStep.hh"
#include "step/player/PlayerLevelUpDivinityStep.hh"

using namespace octopus;

void genStep(std::vector<Steppable *> &steppables_p, BuffOption const &option_p)
{
    steppables_p.push_back(new PlayerBuffAllStep(option_p._player, option_p._buff, option_p._model));
}

void genStep(std::vector<Steppable *> &steppables_p, DoubleBuffOption const &option_p)
{
    steppables_p.push_back(new PlayerBuffAllStep(option_p._player, option_p._buff1, option_p._model));
    steppables_p.push_back(new PlayerBuffAllStep(option_p._player, option_p._buff2, option_p._model));
}

void genStep(std::vector<Steppable *> &steppables_p, ModifierOption const &option_p)
{
    steppables_p.push_back(new PlayerAttackModAllStep(option_p._player, option_p._mod, option_p._model));
}

void genStep(std::vector<Steppable *> &steppables_p, DivinityOption const &option_p)
{
    steppables_p.push_back(new PlayerLevelUpDivinityStep(option_p._player, option_p._div));
}

std::vector<Steppable *> BuffGenerator::getSteppables(unsigned long options_p) const
{
    std::vector<Steppable *> steps_l;
    std::visit([&steps_l](auto &&arg) { genStep(steps_l, arg); }, _options.at(options_p)._playerOption);
    std::visit([&steps_l](auto &&arg) { genStep(steps_l, arg); }, _options.at(options_p)._enemyOption);
    return steps_l;
}

std::string generateRandomModel(RandomGenerator &gen_p)
{
    int model_l = gen_p.roll(0, 2);

    if(model_l == 0)
    {
        return "square";
    }
    else if(model_l == 1)
    {
        return "circle";
    }
    else
    {
        return "triangle";
    }
}

BuffOption generateRandomBuffOptionForEnemy(unsigned long player_p, RandomGenerator &gen_p, std::string const &id_p)
{
    int type_l = gen_p.roll(0, 4);

    BuffOption option_l;
    option_l._player = player_p;

    bool offset_l = false;
    int min_l = 5;
    int max_l = 15;

    if(type_l == 0)
    {
        option_l._buff._type = TyppedBuff::Type::Speed;
    }
    else if(type_l == 1)
    {
        option_l._buff._type = TyppedBuff::Type::FullReload;
    }
    else if(type_l == 2)
    {
        option_l._buff._type = TyppedBuff::Type::Damage;
        offset_l = true;
        min_l = 3;
        max_l = 6;
    }
    else if(type_l == 3)
    {
        option_l._buff._type = TyppedBuff::Type::Armor;
        offset_l = true;
        min_l = 1;
        max_l = 3;
    }
    else if(type_l == 4)
    {
        option_l._buff._type = TyppedBuff::Type::HpMax;
    }

    if(offset_l)
    {
        option_l._buff._offset = gen_p.roll(min_l, max_l);
    }
    else
    {
        option_l._buff._coef = gen_p.roll(min_l, max_l)/100.;
    }

    if(option_l._buff._type == TyppedBuff::Type::FullReload)
    {
        option_l._buff._coef = -option_l._buff._coef;
    }

    option_l._buff._id = id_p;
    option_l._model = generateRandomModel(gen_p);

    return option_l;
}

std::vector<SingleOption> getClassicOptions(unsigned long player_p, std::string const &id_p, bool tierTwo_p, bool rare_p)
{
    std::vector<SingleOption> commons_l;
    std::vector<std::string> models_l = {"square", "triangle", "circle"};
    if(tierTwo_p)
    {
        models_l.push_back("double_square");
        models_l.push_back("reverse_triangle");
    }
    for(std::string const &model_l : models_l)
    {
        BuffOption option_l;
        option_l._player = player_p;
        option_l._buff._type = TyppedBuff::Type::Damage;
        option_l._buff._offset = rare_p?2:1;
        option_l._buff._id = id_p;
        option_l._model = "square";
        commons_l.push_back(option_l);
    }
    for(std::string const &model_l : models_l)
    {
        BuffOption option_l;
        option_l._player = player_p;
        option_l._buff._type = TyppedBuff::Type::Armor;
        option_l._buff._offset = rare_p?2:1;
        option_l._buff._id = id_p;
        option_l._model = "square";
        commons_l.push_back(option_l);
    }

    return commons_l;
}

std::vector<SingleOption> getCommonOptions(unsigned long player_p, std::string const &id_p, bool tierTwo_p)
{
    return getClassicOptions(player_p, id_p, tierTwo_p, false);
}

std::vector<SingleOption> getRareOptions(unsigned long player_p, std::string const &id_p, bool tierTwo_p)
{
    return getClassicOptions(player_p, id_p, tierTwo_p, true);
}

SingleOption generatePlayerOption(octopus::State const &state_p, unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p, bool rare_p)
{
    SingleOption result_l;
    int type_l = gen_p.roll(0, 10);
    // rare option
    if(rare_p || type_l >= 7)
    {
        std::vector<SingleOption> options_l = getRareOptions(player_p, id_p, false);
        int roll_l = gen_p.roll(0, options_l.size());
        result_l = options_l.at(roll_l);
    }
    else
    {
        std::vector<SingleOption> options_l = getCommonOptions(player_p, id_p, false);
        int roll_l = gen_p.roll(0, options_l.size());
        result_l = options_l.at(roll_l);
    }
    return result_l;
}

SingleOption generateEnemyOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p)
{
    return generateRandomBuffOptionForEnemy(player_p, gen_p, id_p);
}
