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

#include "anchor_trigger/AnchorTriggerOptionGenerator.hh"

AnchorTrigger::AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, double bonus_p,
    unsigned long player_p, std::vector<fas::DivinityType> const &forbidden_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), player_p)),
    _player(player_p),
    _lib(lib_p),
    _rand(rand_p),
    _bonus(bonus_p),
    _forbidden(forbidden_p)
{}

std::vector<Option> generateOpeningOptions(
	std::string const &id_p,
	unsigned long seed_p,
	std::vector<fas::DivinityType> const &forbidden_p,
	unsigned long playerIdx_p,
	octopus::State const &state_p)
{
	std::vector<Option> options_l;
	octopus::RandomGenerator rand_l(seed_p);
	Option divOption_l;

	octopus::Player const &player_l = *state_p.getPlayer(playerIdx_p);
	std::vector<fas::DivinityType> exceptions_l = fas::getMaxedOutDivinities(player_l);
	exceptions_l.insert(exceptions_l.end(), forbidden_p.begin(), forbidden_p.end());

	fas::DivinityType firstType_l = fas::rollOneRandomDivinity(rand_l, exceptions_l);
	if(firstType_l != fas::DivinityType::None)
	{
		divOption_l._playerOption = DivinityOption {playerIdx_p, firstType_l};
		options_l.push_back(divOption_l);
		exceptions_l.push_back(firstType_l);
	}

	fas::DivinityType secondType_l = fas::rollOneRandomDivinity(rand_l, exceptions_l);
	if(secondType_l != fas::DivinityType::None)
	{
		divOption_l._playerOption = DivinityOption {playerIdx_p, secondType_l};
		options_l.push_back(divOption_l);
		exceptions_l.push_back(secondType_l);
	}

	fas::DivinityType thirdType_l = fas::rollOneRandomDivinity(rand_l, exceptions_l);
	if(thirdType_l != fas::DivinityType::None)
	{
		divOption_l._playerOption = DivinityOption {playerIdx_p, thirdType_l};
		options_l.push_back(divOption_l);
		exceptions_l.push_back(thirdType_l);
	}

	return options_l;
}

std::vector<Option> generateOptions(
	std::string const &id_p,
	unsigned long seed_p,
	std::vector<fas::DivinityType> const &forbidden_p,
	unsigned long playerIdx_p,
	octopus::State const &state_p)
{
	std::vector<Option> options_l;
	octopus::RandomGenerator rand_l(seed_p);

	// generate all possible option generator associated with their probability
	// - generator
	// - proba coef
	// - allow_multiple
	std::list<AnchorOptionGenerator> generators_l = generateOptionGenerators(state_p, playerIdx_p);

	size_t nb_rolls_l = 3;
	// for each roll
	for(size_t roll_l = 0 ; roll_l < nb_rolls_l ; ++ roll_l)
	{
		// get the sum of all options coef in the pool
		unsigned long sum_proba_l = getSumOfProba(generators_l);
		// roll random number between 0 and sum of
		unsigned long rolled_number_l = rand_l.roll(0, int(sum_proba_l));
		// set up threshold = 0
		unsigned long threshold_l = 0;
		// iterate on all option generator in the pool
		for(auto it_l = generators_l.begin() ; it_l != generators_l.end() ; ++it_l)
		{
			AnchorOptionGenerator const &generator_l = *it_l;
			// increment threshold with the current proba coef
			threshold_l += generator_l.proba_coef;
			// if rolled number < threshold
			if(rolled_number_l <= threshold_l)
			{
				// generate option
				options_l.push_back(generator_l.generator(state_p, playerIdx_p, rand_l, id_p));
				// if current option does not allow multiple
				if(!generator_l.allow_multiple)
				{
					// clear option generator from pool
					generators_l.erase(it_l);
				}
				// break inner loop
				break;
			}

		}
	}
	return options_l;
}

void AnchorTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long idx_p, octopus::TriggerData const &data_p) const
{
    std::map<std::string, octopus::Fixed> map_l;
    map_l["Anchor"] = -_bonus;
    step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));

    std::string id_l = std::to_string(++_count);
	// roll seed for generation
	unsigned long seed_l = _rand.roll(0,std::numeric_limits<int>::max());

	std::function<std::vector<Option>(octopus::State const &)> optionsGenerator_l =
		[id_l, seed_l, forbidden_l = _forbidden, playerIdx_l = _player](octopus::State const & s)
		{
			return generateOptions(id_l, seed_l, forbidden_l, playerIdx_l, s);
		};

	octopus::ListenerEntityData const * listenerData_l = dynamic_cast<octopus::ListenerEntityData const *>(data_p._listenerData[0]);
    step_p.addSteppable(new octopus::StateTemplePositionRemoveStep(listenerData_l->_entities.at(idx_p)->_pos));

    step_p.addSteppable(new octopus::PlayerAddOptionStep(_player, id_l, new BuffGenerator(id_l, optionsGenerator_l, _lib)));
}
