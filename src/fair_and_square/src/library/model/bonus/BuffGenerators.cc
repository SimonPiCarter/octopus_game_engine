#include "BuffGenerators.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerAttackModAllStep.hh"
#include "step/player/PlayerLevelUpUpgradeStep.hh"

using namespace octopus;

void genStep(Library const &lib_p, std::vector<Steppable *> &steppables_p, BuffOption const &option_p)
{
    steppables_p.push_back(new PlayerBuffAllStep(option_p._player, option_p._buff, option_p._model));
    if(option_p._div != "")
    {
        steppables_p.push_back(new PlayerLevelUpUpgradeStep(option_p._player, option_p._div));
    }
}

void genStep(Library const &lib_p, std::vector<Steppable *> &steppables_p, DoubleBuffOption const &option_p)
{
    steppables_p.push_back(new PlayerBuffAllStep(option_p._player, option_p._buff1, option_p._model));
    steppables_p.push_back(new PlayerBuffAllStep(option_p._player, option_p._buff2, option_p._model));
}

void genStep(Library const &lib_p, std::vector<Steppable *> &steppables_p, ModifierOption const &option_p)
{
    steppables_p.push_back(new PlayerAttackModAllStep(option_p._player, option_p._mod, option_p._model));
}

void genStep(Library const &lib_p, std::vector<Steppable *> &steppables_p, ::DivinityOption const &option_p)
{
    std::vector<Steppable *> playerBuilding_l = newPlayerBuilding(option_p._player, option_p._div, lib_p);
    for(Steppable * steppable_l : playerBuilding_l)
    {
        steppables_p.push_back(steppable_l);
    }
    steppables_p.push_back(new PlayerLevelUpUpgradeStep(option_p._player, fas::divinityUpgradeName(option_p._div)));
}

std::vector<Steppable *> BuffGenerator::getSteppables(unsigned long options_p) const
{
    std::vector<Steppable *> steps_l;
    std::visit([this, &steps_l](auto &&arg) { genStep(_lib, steps_l, arg); }, _options.at(options_p)._playerOption);
    std::visit([this, &steps_l](auto &&arg) { genStep(_lib, steps_l, arg); }, _options.at(options_p)._enemyOption);
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
        option_l._model = model_l;
        commons_l.push_back(option_l);
    }
    for(std::string const &model_l : models_l)
    {
        BuffOption option_l;
        option_l._player = player_p;
        option_l._buff._type = TyppedBuff::Type::Armor;
        option_l._buff._offset = rare_p?2:1;
        option_l._buff._id = id_p;
        option_l._model = model_l;
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

std::vector<SingleOption> getEpicOptions(octopus::Player const &player_p, std::string const &id_p, bool tierTwo_p, bool tierThree_p, bool disabledBuffer_p)
{
    std::vector<SingleOption> options_l;

    std::vector<std::string> models_l = {"square", "triangle", "circle"};
    if(tierTwo_p)
    {
        models_l.push_back("double_square");
        models_l.push_back("reverse_triangle");
    }
    if(tierThree_p)
    {
        models_l.push_back("long_square");
        models_l.push_back("long_triangle");
    }

    // epic options are unique so we check levels to know if player used it already or not

    // reload epic option
    for(std::string const &model_l : models_l)
    {
        std::string levelName_p = model_l+"_reload";
        // not chosen yet
        if(octopus::getUpgradeLvl(player_p, levelName_p) == 0)
        {
            BuffOption option_l;
            option_l._player = player_p._id;
            option_l._buff._type = TyppedBuff::Type::FullReload;
            option_l._buff._coef = -0.5;
            option_l._buff._id = id_p;
            option_l._model = model_l;
            option_l._div = levelName_p;
            options_l.push_back(option_l);
        }
    }

    // double damage epic option
    models_l = {"long_square", "long_triangle"};
    for(std::string const &model_l : models_l)
    {
        std::string levelName_p = model_l+"_double_damage";
        // not chosen yet
        if(octopus::getUpgradeLvl(player_p, levelName_p) == 0)
        {
            BuffOption option_l;
            option_l._player = player_p._id;
            option_l._buff._type = TyppedBuff::Type::Damage;
            option_l._buff._coef = 1;
            option_l._buff._id = id_p;
            option_l._model = model_l;
            option_l._div = levelName_p;
            options_l.push_back(option_l);
        }
    }

    return options_l;
}

SingleOption generatePlayerOption(octopus::State const &state_p, unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p, bool rare_p)
{
    SingleOption result_l;
    int type_l = gen_p.roll(1, 100);
    octopus::Player const * player_l = state_p.getPlayer(player_p);
    bool tierTwo_l = octopus::getUpgradeLvl(*player_l, "tier") > 1;
    bool tierThree_l = octopus::getUpgradeLvl(*player_l, "tier") > 2;
    std::vector<SingleOption> optionsEpic_l = getEpicOptions(*player_l, id_p, tierTwo_l, tierThree_l, true);
    // epic option : rare 20% / non rare 5%
    if(optionsEpic_l.size() > 0
    && ((rare_p && type_l <= 20) || type_l <= 5))
    {
        int roll_l = gen_p.roll(0, optionsEpic_l.size()-1);
        result_l = optionsEpic_l.at(roll_l);
    }
    // rare option : rare 100% / non rare 30% (5% epic)
    else if(rare_p || type_l <= 30)
    {
        std::vector<SingleOption> options_l = getRareOptions(player_p, id_p, tierTwo_l);
        int roll_l = gen_p.roll(0, options_l.size()-1);
        result_l = options_l.at(roll_l);
    }
    // common
    else
    {
        std::vector<SingleOption> options_l = getCommonOptions(player_p, id_p, tierTwo_l);
        int roll_l = gen_p.roll(0, options_l.size()-1);
        result_l = options_l.at(roll_l);
    }
    return result_l;
}

SingleOption generateEnemyOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p)
{
    return generateRandomBuffOptionForEnemy(player_p, gen_p, id_p);
}
