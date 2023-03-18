#include "BuffGenerators.hh"

#include "step/player/PlayerBuffAllStep.hh"

using namespace octopus;

BuffGenerator::~BuffGenerator()
{
    for(auto step_l : _steps)
    {
        delete step_l;
    }
}

std::vector<Steppable *> const &BuffGenerator::getSteppables(unsigned long options_p, unsigned long player_p)
{
    if(_steps.empty())
    {
        _steps.push_back(new PlayerBuffAllStep(player_p, _options.at(options_p)._buff, _options.at(options_p)._model));
    }
    return _steps;
}

BuffOption generateRandomOption(std::mt19937 &gen_p, std::string const &id_p)
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
    && option_l._buff._type != TyppedBuff::Type::Speed)
    {
        std::uniform_int_distribution<> dist_l(3, 6);
        option_l._buff._offset = dist_l(gen_p);
    }
    else
    {
        std::uniform_int_distribution<> dist_l(5, 15);
        option_l._buff._coef = dist_l(gen_p)/100.;
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
        std::uniform_int_distribution<> distModel_l(0, 2);
        int model_l = distModel_l(gen_p);

        if(model_l == 0)
        {
            option_l._model = "square";
        }
        else if(model_l == 1)
        {
            option_l._model = "circle";
        }
        else if(model_l == 2)
        {
            option_l._model = "triangle";
        }
    }

    return option_l;
}
