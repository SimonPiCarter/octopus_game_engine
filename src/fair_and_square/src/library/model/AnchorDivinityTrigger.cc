#include "AnchorDivinityTrigger.hh"

#include <limits>

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
#include "library/model/divinity/DivinityModelLoader.hh"

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
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

	// roll seed for generation
	unsigned long seed_l = _rand.roll(0,std::numeric_limits<int>::max());

	std::function<std::vector<fas::DivinityType>(octopus::State const &)> optionGenerator_l =
		[types_l = _types, seed_l, playerIdx_l = _player](octopus::State const &s)
		{
			octopus::Player const &player_l = *s.getPlayer(playerIdx_l);
			octopus::RandomGenerator rand_l(seed_l);
			std::vector<fas::DivinityType> options_l = fas::getMaxedOutDivinities(player_l);
			for(int i = 0 ; i < 3 ; ++ i)
			{
				fas::DivinityType roll_l = rollOneRandomDivinityFromList(rand_l, types_l, options_l);
				if(roll_l != fas::DivinityType::None)
				{
					options_l.push_back(roll_l);
				}
			}
			return options_l;
		};

	octopus::ListenerEntityData const * listenerData_l = dynamic_cast<octopus::ListenerEntityData const *>(data_p._listenerData[0]);
    step_p.addSteppable(new octopus::StateTemplePositionRemoveStep(listenerData_l->_entities.at(idx_p)->_pos));

    std::string id_l = std::to_string(_count++);
    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new DivinityGenerator(_player, optionGenerator_l)));
}
