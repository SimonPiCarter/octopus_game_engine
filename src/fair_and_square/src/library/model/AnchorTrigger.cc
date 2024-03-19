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

AnchorTrigger::AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, double bonus_p,
    unsigned long player_p, std::vector<fas::DivinityType> const &forbidden_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), player_p)),
    _player(player_p),
    _lib(lib_p),
    _rand(rand_p),
    _bonus(bonus_p),
    _forbidden(forbidden_p)
{}

void AnchorTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long idx_p, octopus::TriggerData const &data_p) const
{
    std::map<std::string, octopus::Fixed> map_l;
    map_l["Anchor"] = -_bonus;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

    std::string id_l = std::to_string(_count++);
	// roll seed for generation
	unsigned long seed_l = _rand.roll(0,std::numeric_limits<int>::max());

	std::function<std::vector<Option>(octopus::State const &)> optionsGenerator_l =
		[id_l, seed_l, forbidden_l = _forbidden, playerIdx_l = _player](octopus::State const & s)
		{
			octopus::RandomGenerator rand_l(seed_l);
			std::vector<Option> options_l;

			// div option
			Option divOption_l;

			//
			octopus::Player const &player_l = *s.getPlayer(playerIdx_l);
			std::vector<fas::DivinityType> exceptions_l = fas::getMaxedOutDivinities(player_l);
			exceptions_l.insert( exceptions_l.end(), forbidden_l.begin(), forbidden_l.end() );

			fas::DivinityType firstType_l = fas::rollOneRandomDivinity(rand_l, exceptions_l);
			if(firstType_l != fas::DivinityType::None)
			{
				divOption_l._playerOption = DivinityOption {playerIdx_l, firstType_l};
				divOption_l._enemyOption = generateEnemyOption((playerIdx_l+1)%2, rand_l, id_l);
				options_l.push_back(divOption_l);
				// add exception to avoid double roll of the same
				exceptions_l.push_back(firstType_l);
			}

			fas::DivinityType secondType_l = fas::rollOneRandomDivinity(rand_l, exceptions_l);
			if(secondType_l != fas::DivinityType::None)
			{
				divOption_l._playerOption = DivinityOption {playerIdx_l, secondType_l};
				divOption_l._enemyOption = generateEnemyOption((playerIdx_l+1)%2, rand_l, id_l);
				options_l.push_back(divOption_l);
			}

			for(uint32_t i = 0 ; i < 3-options_l.size() ; ++ i )
			{
				Option option_l;

				option_l._playerOption = generatePlayerOption(s, playerIdx_l, rand_l, id_l, i==2);
				option_l._enemyOption = generateEnemyOption((playerIdx_l+1)%2, rand_l, id_l);

				options_l.push_back(option_l);
			}

			return options_l;
		};

	octopus::ListenerEntityData const * listenerData_l = dynamic_cast<octopus::ListenerEntityData const *>(data_p._listenerData[0]);
    step_p.addSteppable(new octopus::StateTemplePositionRemoveStep(listenerData_l->_entities.at(idx_p)->_pos));

    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new BuffGenerator(id_l, optionsGenerator_l, _lib)));
}
