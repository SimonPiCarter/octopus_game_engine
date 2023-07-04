#include "AnchorDivinityTrigger.hh"

#include "library/Library.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "state/model/entity/BuildingModel.hh"

#include "library/model/divinity/DivinityGenerator.hh"

AnchorDivinityTrigger::AnchorDivinityTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, unsigned long player_p, std::vector<fas::DivinityType> const &types_p, double bonus_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), player_p)),
    _player(player_p),
    _rand(rand_p),
    _types(types_p),
    _bonus(bonus_p)
{}

std::vector<int> roll_with_no_doublon(octopus::RandomGenerator &rand_p, int min_p, int max_p, int number_p)
{
    if (max_p - min_p <= 0)
    {
        return {};
    }
    else if (max_p - min_p >= number_p)
    {
        std::vector<int> res_l;
        for(int i = min_p ; i < max_p ; ++i)
        {
            res_l.push_back(i);
        }
        return res_l;
    }
    std::vector<int> rolls_l;

    for(int i = 0 ; i < number_p ; ++ i )
    {
        int roll_l = rand_p.roll(min_p, max_p-1-i);

        int offset_l = 0;
        // increment roll for every previously roll this one is equal or greater
        for(size_t j = 0 ; j < rolls_l.size() ; ++ j)
        {
            if(roll_l >= rolls_l.at(j))
            {
                ++offset_l;
            }
        }

        rolls_l.push_back(roll_l+offset_l);
    }

    return rolls_l;
}

void AnchorDivinityTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
    std::map<std::string, octopus::Fixed> map_l;
    map_l["Anchor"] = -_bonus;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

    std::vector<int> rolls_l = roll_with_no_doublon(_rand, 0, _types.size(), 3);
    std::vector<fas::DivinityType> options_l;
    for(int i = 0 ; i < 3 ; ++ i)
    {
        options_l.push_back(_types.at(rolls_l.at(i)));
    }

    std::string id_l = std::to_string(_count++);
    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new DivinityGenerator(_player, options_l)));
}
