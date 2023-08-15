#include "AnchorTrigger.hh"

#include "library/Library.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionRemoveStep.hh"
#include "state/model/entity/BuildingModel.hh"
#include "controller/trigger/Listener.hh"

#include "library/model/bonus/BuffGenerators.hh"
#include "library/model/divinity/DivinityModelLoader.hh"

AnchorTrigger::AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, double bonus_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), 0)),
    _player(0),
    _lib(lib_p),
    _rand(rand_p),
    _bonus(bonus_p)
{}

void AnchorTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long idx_p, octopus::TriggerData const &data_p) const
{
    std::map<std::string, octopus::Fixed> map_l;
    map_l["Anchor"] = -_bonus;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

    std::string id_l = std::to_string(_count++);
    std::vector<Option> options_l;

    // div option
    Option divOption_l;

	//
	octopus::Player const &player_l = *state_p.getPlayer(_player);
	std::vector<fas::DivinityType> exceptions_l = fas::getMaxedOutDivinities(player_l);

	fas::DivinityType firstType_l = fas::rollOneRandomDivinity(_rand, exceptions_l);
	if(firstType_l != fas::DivinityType::None)
	{
		divOption_l._playerOption = DivinityOption {_player, firstType_l};
		divOption_l._enemyOption = generateEnemyOption((_player+1)%2, _rand, id_l);
		options_l.push_back(divOption_l);
		// add exception to avoid double roll of the same
		exceptions_l.push_back(firstType_l);
	}


	fas::DivinityType secondType_l = fas::rollOneRandomDivinity(_rand, exceptions_l);
	if(secondType_l != fas::DivinityType::None)
	{
		divOption_l._playerOption = DivinityOption {_player, secondType_l};
		divOption_l._enemyOption = generateEnemyOption((_player+1)%2, _rand, id_l);
		options_l.push_back(divOption_l);
	}

    for(size_t i = 0 ; i < 3-options_l.size() ; ++ i )
    {
        Option option_l;

        option_l._playerOption = generatePlayerOption(state_p, _player, _rand, id_l, i==2);
        option_l._enemyOption = generateEnemyOption((_player+1)%2, _rand, id_l);

        options_l.push_back(option_l);
    }

	octopus::ListenerEntityData const * listenerData_l = dynamic_cast<octopus::ListenerEntityData const *>(data_p._listenerData[0]);
    step_p.addSteppable(new octopus::StateTemplePositionRemoveStep(listenerData_l->_entities.at(idx_p)->_pos));

    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new BuffGenerator(options_l, _lib)));
}
