#include "DivinityGenerator.hh"

#include "generators/UnitModelIds.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerAttackModAllStep.hh"
#include "step/player/PlayerLevelUpUpgradeStep.hh"

using namespace octopus;

void DivinityGenerator::genOptions(State const &state_p)
{
	_options = _optionsGenerator(state_p);
}

std::vector<Steppable *> DivinityGenerator::genSteppables(octopus::State const &state_p, unsigned long options_p) const
{
    std::vector<Steppable *> steps_l;

    fas::DivinityType type_l = _options.at(options_p);

    std::string upgrade_l = divinityUpgradeName(type_l);
    steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, upgrade_l));

    return steps_l;
}
