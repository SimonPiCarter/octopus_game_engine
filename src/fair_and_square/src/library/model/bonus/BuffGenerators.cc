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

TyppedBuff generateBuff(RandomGenerator &gen_p, bool allowBuildingBuff_p)
{
    int type_l = gen_p.roll(0, allowBuildingBuff_p?6:4);
    TyppedBuff buff_l;
    bool offset_l = false;
    int min_l = 5;
    int max_l = 15;

    if(type_l == 0)
    {
        buff_l._type = TyppedBuff::Type::Speed;
    }
    else if(type_l == 1)
    {
        buff_l._type = TyppedBuff::Type::FullReload;
    }
    else if(type_l == 2)
    {
        buff_l._type = TyppedBuff::Type::Damage;
        offset_l = true;
        min_l = 3;
        max_l = 6;
    }
    else if(type_l == 3)
    {
        buff_l._type = TyppedBuff::Type::Armor;
        offset_l = true;
        min_l = 1;
        max_l = 3;
    }
    else if(type_l == 4)
    {
        buff_l._type = TyppedBuff::Type::HpMax;
    }
    else if(type_l == 5)
    {
        buff_l._type = TyppedBuff::Type::Production;
    }
    else if(type_l == 6)
    {
        buff_l._type = TyppedBuff::Type::Harvest;
    }

    if(offset_l)
    {
        buff_l._offset = gen_p.roll(min_l, max_l);
    }
    else
    {
        buff_l._coef = gen_p.roll(min_l, max_l)/100.;
    }

    if(buff_l._type == TyppedBuff::Type::FullReload)
    {
        buff_l._coef = -buff_l._coef;
    }

    return buff_l;
}

TyppedBuff generateDebuff(RandomGenerator &gen_p, bool allowBuildingBuff_p)
{
    int type_l = gen_p.roll(0, allowBuildingBuff_p?6:4);
    TyppedBuff buff_l;
    bool offset_l = false;
    int min_l = 5;
    int max_l = 15;

    if(type_l == 0)
    {
        buff_l._type = TyppedBuff::Type::Speed;
    }
    else if(type_l == 1)
    {
        buff_l._type = TyppedBuff::Type::FullReload;
    }
    else if(type_l == 2)
    {
        buff_l._type = TyppedBuff::Type::Damage;
        offset_l = true;
        min_l = 3;
        max_l = 6;
    }
    else if(type_l == 3)
    {
        buff_l._type = TyppedBuff::Type::Armor;
        offset_l = true;
        min_l = 1;
        max_l = 3;
    }
    else if(type_l == 4)
    {
        buff_l._type = TyppedBuff::Type::HpMax;
    }
    else if(type_l == 5)
    {
        buff_l._type = TyppedBuff::Type::Production;
    }
    else if(type_l == 6)
    {
        buff_l._type = TyppedBuff::Type::Harvest;
    }

    if(offset_l)
    {
        buff_l._offset = -gen_p.roll(min_l, max_l);
    }
    else
    {
        buff_l._coef = -gen_p.roll(min_l, max_l)/100.;
    }

    if(buff_l._type == TyppedBuff::Type::FullReload)
    {
        buff_l._coef = -buff_l._coef;
    }

    return buff_l;
}

BuffOption generateRandomBuffOption(unsigned long player_p, RandomGenerator &gen_p, std::string const &id_p)
{
    BuffOption option_l;
    option_l._player = player_p;

    option_l._buff = generateBuff(gen_p, true);

    option_l._buff._id = id_p;

    if(option_l._buff._type == TyppedBuff::Type::Production)
    {
        option_l._model = "barrack";
    }
    else if(option_l._buff._type == TyppedBuff::Type::Harvest)
    {
        option_l._model = "deposit";
    }
    else
    {
        option_l._model = generateRandomModel(gen_p);
    }

    return option_l;
}

DoubleBuffOption generateRandomDoubleBuffOption(unsigned long player_p, RandomGenerator &gen_p, std::string const &id_p)
{
    DoubleBuffOption option_l;
    option_l._player = player_p;

    option_l._buff1 = generateBuff(gen_p, false);
    // generate until different type
    do
    {
        option_l._buff2 = generateDebuff(gen_p, false);
    } while(option_l._buff2._type == option_l._buff1._type);

    option_l._buff1._id = id_p+"_buff1";
    option_l._buff2._id = id_p+"_buff2";
    option_l._model = generateRandomModel(gen_p);

    return option_l;
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

ModifierOption generateRandomModifierOption(unsigned long player_p, RandomGenerator &gen_p)
{
    ModifierOption option_l;
    option_l._player = player_p;

    option_l._model = generateRandomModel(gen_p);

    int mod_l = gen_p.roll(0, 3);

    // roll power
    Fixed power_l = gen_p.roll(100, 400)/Fixed(100);
    // AoE
    if(mod_l == 0)
    {
        int range_l = gen_p.roll(2,5);
        Fixed ratio_l(1);
        if(range_l == 2) { ratio_l = power_l/4.; }
        if(range_l == 3) { ratio_l = power_l/6.; }
        if(range_l == 4) { ratio_l = power_l/12.; }
        if(range_l == 5) { ratio_l = power_l/24.; }
        option_l._mod = AoEModifier(octopus::to_double(ratio_l), range_l);
    }
    // Chaining
    else if(mod_l == 1)
    {
        int links_l = gen_p.roll(2,5);
        option_l._mod = ChainingModifier(20, links_l, octopus::to_double(power_l/links_l), 5);
    }
    // Dot
    else if(mod_l == 2)
    {
        int ticks_l = gen_p.roll(4,8);
        option_l._mod = DotModifier(100, ticks_l, octopus::to_double(10*power_l/ticks_l));
    }
    // Lifesteal
    else
    {
        option_l._mod = LifeStealModifier(octopus::to_double(power_l/10.));
    }

    return option_l;
}

SingleOption generatePlayerOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p)
{
    int type_l = gen_p.roll(0, 10);
    if(type_l==0)
    {
        return DivinityOption {player_p, octopus::DivinityType::Divinity_1};
    }
    if(type_l==1)
    {
        return DivinityOption {player_p, octopus::DivinityType::Divinity_2};
    }
    type_l = gen_p.roll(0, 4);
    if(type_l==0)
    {
        return generateRandomModifierOption(player_p, gen_p);
    }
    if(type_l==1)
    {
        return generateRandomDoubleBuffOption(player_p, gen_p, id_p);
    }
    return generateRandomBuffOption(player_p, gen_p, id_p);
}

SingleOption generateEnemyOption(unsigned long player_p, octopus::RandomGenerator &gen_p, std::string const &id_p)
{
    return generateRandomBuffOptionForEnemy(player_p, gen_p, id_p);
}
