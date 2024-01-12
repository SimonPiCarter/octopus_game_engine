#include "AnchorDivinityTrigger.hh"

#include "library/Library.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionRemoveStep.hh"
#include "state/model/entity/BuildingModel.hh"
#include "controller/trigger/Listener.hh"

#include "library/model/divinity/DivinityGenerator.hh"

AnchorDivinityTrigger::AnchorDivinityTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, unsigned long player_p, std::vector<fas::DivinityType> const &types_p, double bonus_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), player_p)),
    _player(player_p),
    _rand(rand_p),
    _types(types_p),
    _bonus(bonus_p)
{}

void AnchorDivinityTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long idx_p, octopus::TriggerData const &data_p) const
{
    std::map<std::string, octopus::Fixed> map_l;
    map_l["Anchor"] = -_bonus;
    step_p.addSteppable(state_p, new octopus::PlayerSpendResourceStep(_player, map_l));

    std::vector<fas::DivinityType> options_l;
    for(int i = 0 ; i < 3 ; ++ i)
    {
		fas::DivinityType roll_l = rollOneRandomDivinityFromList(_rand, _types, options_l);
		if(roll_l != fas::DivinityType::None)
		{
        	options_l.push_back(roll_l);
		}
    }

	octopus::ListenerEntityData const * listenerData_l = dynamic_cast<octopus::ListenerEntityData const *>(data_p._listenerData[0]);
    step_p.addSteppable(state_p, new octopus::StateTemplePositionRemoveStep(listenerData_l->_entities.at(idx_p)->_pos));

    std::string id_l = std::to_string(_count++);
    step_p.addSteppable(state_p, new octopus::PlayerAddOptionStep(_player, id_l, new DivinityGenerator(_player, options_l)));
}
