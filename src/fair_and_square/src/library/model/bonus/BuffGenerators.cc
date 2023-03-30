#include "BuffGenerators.hh"

#include "state/entity/attackModifier/AttackModifier.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerAttackModAllStep.hh"

using namespace octopus;

Steppable * genStep(unsigned long player_p, BuffOption const &option_p)
{
    return new PlayerBuffAllStep(player_p, option_p._buff, option_p._model);
}

Steppable * genStep(unsigned long player_p, ModifierOption const &option_p)
{
    return new PlayerAttackModAllStep(player_p, option_p._mod, option_p._model);
}

std::vector<Steppable *> BuffGenerator::getSteppables(unsigned long options_p, unsigned long player_p) const
{
    std::vector<Steppable *> steps_l;
    std::visit([&steps_l, &player_p](auto &&arg) { steps_l.push_back(genStep(player_p, arg)); }, _options.at(options_p));
    return steps_l;
}

std::string generateRandomModel(std::mt19937 &gen_p)
{
    std::uniform_int_distribution<> distModel_l(0, 2);
    int model_l = distModel_l(gen_p);

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

BuffOption generateRandomBuffOption(std::mt19937 &gen_p, std::string const &id_p)
{
    std::uniform_int_distribution<> distType_l(0, 6);
    int type_l = distType_l(gen_p);

    BuffOption option_l;

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
    }
    else if(type_l == 3)
    {
        option_l._buff._type = TyppedBuff::Type::Armor;
    }
    else if(type_l == 4)
    {
        option_l._buff._type = TyppedBuff::Type::HpMax;
    }
    else if(type_l == 5)
    {
        option_l._buff._type = TyppedBuff::Type::Production;
    }
    else if(type_l == 6)
    {
        option_l._buff._type = TyppedBuff::Type::Harvest;
    }

    std::uniform_int_distribution<> distBonusType_l(0, 1);
    int bonusType_l = distBonusType_l(gen_p);

    if(bonusType_l == 0
    && option_l._buff._type != TyppedBuff::Type::Production
    && option_l._buff._type != TyppedBuff::Type::Harvest
    && option_l._buff._type != TyppedBuff::Type::Speed
    && option_l._buff._type != TyppedBuff::Type::FullReload)
    {
        std::uniform_int_distribution<> dist_l(3, 6);
        option_l._buff._offset = dist_l(gen_p);
    }
    else
    {
        std::uniform_int_distribution<> dist_l(5, 15);
        option_l._buff._coef = dist_l(gen_p)/100.;
    }

    if(option_l._buff._type == TyppedBuff::Type::FullReload)
    {
        option_l._buff._coef = -option_l._buff._coef;
    }

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

ModifierOption generateRandomModifierOption(std::mt19937 &gen_p)
{
    ModifierOption option_l;

    option_l._model = generateRandomModel(gen_p);

    std::uniform_int_distribution<> distMod_l(0, 3);
    int mod_l = distMod_l(gen_p);

    if(mod_l == 0)
    {
        std::uniform_int_distribution<> distRange_l(2, 4);
        std::uniform_int_distribution<> distRatio_l(2, 5);
        option_l._mod = AoEModifier(distRatio_l(gen_p)/10., distRange_l(gen_p));
    }
    else if(mod_l == 1)
    {
        std::uniform_int_distribution<> distTicks_l(2, 5);
        std::uniform_int_distribution<> distRatio_l(2, 5);
        std::uniform_int_distribution<> distRange_l(2, 5);
        option_l._mod = ChainingModifier(20, distTicks_l(gen_p), distRatio_l(gen_p)/10., distRange_l(gen_p));
    }
    else if(mod_l == 2)
    {
        std::uniform_int_distribution<> distTicks_l(2, 5);
        std::uniform_int_distribution<> distDmg_l(5, 10);
        option_l._mod = DotModifier(100, distTicks_l(gen_p), distDmg_l(gen_p));
    }
    else
    {
        std::uniform_int_distribution<> distRatio_l(2, 4);
        option_l._mod = LifeStealModifier(distRatio_l(gen_p)/10.);
    }

    return option_l;
}
