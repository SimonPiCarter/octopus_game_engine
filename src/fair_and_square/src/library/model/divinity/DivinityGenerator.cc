#include "DivinityGenerator.hh"

#include "generators/UnitModelIds.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerAttackModAllStep.hh"
#include "step/player/PlayerLevelUpUpgradeStep.hh"

using namespace octopus;

std::vector<Steppable *> DivinityGenerator::getSteppables(unsigned long options_p) const
{
    std::vector<Steppable *> steps_l;

    fas::DivinityType type_l = _options.at(options_p);

    switch(type_l)
    {
        case fas::DivinityType::AttackSpeed:
            steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, models::AttackSpeedDivId));
            break;
        case fas::DivinityType::Economic:
            steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, models::EconomicDivId));
            break;
        case fas::DivinityType::Heal:
            steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, models::HealDivId));
            break;
        case fas::DivinityType::Lifesteal:
            steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, models::LifestealDivId));
            break;
        case fas::DivinityType::Production:
            steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, models::ProductionDivId));
            break;
        case fas::DivinityType::Recycle:
            steps_l.push_back(new PlayerLevelUpUpgradeStep(_player, models::RecycleDivId));
            break;
    }

    return steps_l;
}
