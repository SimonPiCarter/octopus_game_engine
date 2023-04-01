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

BuffOption generateRandomBuffOption(RandomGenerator &gen_p, std::string const &id_p)
{
    int type_l = gen_p.roll(0, 6);

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

    int bonusType_l = gen_p.roll(0, 1);

    if(bonusType_l == 0
    && option_l._buff._type != TyppedBuff::Type::Production
    && option_l._buff._type != TyppedBuff::Type::Harvest
    && option_l._buff._type != TyppedBuff::Type::Speed
    && option_l._buff._type != TyppedBuff::Type::FullReload)
    {
        option_l._buff._offset = gen_p.roll(3, 6);
    }
    else
    {
        option_l._buff._coef = gen_p.roll(5, 15)/100.;
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

ModifierOption generateRandomModifierOption(RandomGenerator &gen_p)
{
    ModifierOption option_l;

    option_l._model = generateRandomModel(gen_p);

    int mod_l = gen_p.roll(0, 3);

    if(mod_l == 0)
    {
        option_l._mod = AoEModifier(gen_p.roll(2, 4)/10., gen_p.roll(2, 5));
    }
    else if(mod_l == 1)
    {
        option_l._mod = ChainingModifier(20, gen_p.roll(2, 5), gen_p.roll(2, 5)/10., gen_p.roll(2, 5));
    }
    else if(mod_l == 2)
    {
        option_l._mod = DotModifier(100, gen_p.roll(2, 5), gen_p.roll(5, 10));
    }
    else
    {
        option_l._mod = LifeStealModifier(gen_p.roll(2, 4)/10.);
    }

    return option_l;
}
