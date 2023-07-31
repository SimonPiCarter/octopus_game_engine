#include "AnchorTrigger.hh"

#include "library/Library.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "state/model/entity/BuildingModel.hh"

#include "library/model/bonus/BuffGenerators.hh"
#include "library/model/divinity/DivinityModelLoader.hh"

AnchorTrigger::AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, double bonus_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), 0)),
    _player(0),
    _lib(lib_p),
    _rand(rand_p),
    _bonus(bonus_p)
{}

void AnchorTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
    std::map<std::string, octopus::Fixed> map_l;
    map_l["Anchor"] = -_bonus;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

    std::string id_l = std::to_string(_count++);
    std::vector<Option> options_l;

    // div option
    Option divOption_l;

    divOption_l._playerOption = DivinityOption {_player, fas::rollOneRandomDivinity(_rand)};
    divOption_l._enemyOption = generateEnemyOption((_player+1)%2, _rand, id_l);

    options_l.push_back(divOption_l);

    divOption_l._playerOption = DivinityOption {_player, fas::rollOneRandomDivinity(_rand)};
    divOption_l._enemyOption = generateEnemyOption((_player+1)%2, _rand, id_l);

    options_l.push_back(divOption_l);

    for(size_t i = 0 ; i < 1 ; ++ i )
    {
        Option option_l;

        option_l._playerOption = generatePlayerOption(state_p, _player, _rand, id_l, i==2);
        option_l._enemyOption = generateEnemyOption((_player+1)%2, _rand, id_l);

        options_l.push_back(option_l);
    }

    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new BuffGenerator(options_l, _lib)));
}
