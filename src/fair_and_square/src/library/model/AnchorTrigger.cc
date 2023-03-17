#include "AnchorTrigger.hh"

#include "library/Library.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "state/model/entity/BuildingModel.hh"

#include "library/model/bonus/BuffGenerators.hh"

AnchorTrigger::AnchorTrigger(octopus::Library const &lib_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), 0)),
    _player(0),
    _gen(42)
{}

void AnchorTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long) const
{
    std::map<octopus::ResourceType, double> map_l;
    map_l[octopus::ResourceType::Anchor] = -240;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

    std::string id_l = std::to_string(state_p.getPathGridStatus());
    std::vector<BuffOption> options_l;
    for(size_t i = 0 ; i < 3 ; ++ i )
    {
        options_l.push_back(generateRandomOption(_gen, id_l));
    }

    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new BuffGenerator(options_l)));
}