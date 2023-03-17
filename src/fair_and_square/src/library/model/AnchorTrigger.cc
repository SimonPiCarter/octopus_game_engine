#include "AnchorTrigger.hh"

#include "library/Library.hh"
#include "step/Step.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "state/model/entity/BuildingModel.hh"

AnchorTrigger::AnchorTrigger(octopus::Library const &lib_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), 0)),
    _player(0),
    _gen(42)
{}

void AnchorTrigger::trigger(octopus::State const &, octopus::Step &step_p, unsigned long) const
{
    std::map<octopus::ResourceType, double> map_l;
    map_l[octopus::ResourceType::Anchor] = -240;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));
}
