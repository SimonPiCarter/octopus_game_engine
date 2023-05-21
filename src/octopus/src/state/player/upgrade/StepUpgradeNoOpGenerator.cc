#include "StepUpgradeNoOpGenerator.hh"

namespace octopus
{

StepUpgradeNoOpGenerator::~StepUpgradeNoOpGenerator()
{}

StepUpgradeGenerator* StepUpgradeNoOpGenerator::newCopy() const
{
    return new StepUpgradeNoOpGenerator();
}

std::vector<Steppable *> StepUpgradeNoOpGenerator::getSteppables(unsigned long player_p) const
{
    return {};
}

}// octopus
