#include "StepUpgradeTyppedBuffGenerator.hh"

#include "step/player/PlayerBuffAllStep.hh"

namespace octopus
{

StepUpgradeTyppedBuffGenerator::StepUpgradeTyppedBuffGenerator(TimedBuff const &buff_p, std::vector<std::string> const &models_p)
    : _buffs({buff_p}), _models(models_p)
{}
StepUpgradeTyppedBuffGenerator::StepUpgradeTyppedBuffGenerator(std::vector<TimedBuff> const &buffs_p, std::vector<std::string> const &models_p)
    : _buffs(buffs_p), _models(models_p)
{}
StepUpgradeTyppedBuffGenerator::~StepUpgradeTyppedBuffGenerator()
{}

StepUpgradeGenerator* StepUpgradeTyppedBuffGenerator::newCopy() const
{
    return new StepUpgradeTyppedBuffGenerator(_buffs, _models);
}

std::vector<Steppable *> StepUpgradeTyppedBuffGenerator::getSteppables(unsigned long player_p, unsigned long) const
{
    std::vector<Steppable *> vect_l;

    for(std::string const &model_l : _models)
    {
        for(TimedBuff const buff_l : _buffs)
        {
            vect_l.push_back(new PlayerBuffAllStep(player_p, buff_l, model_l));
        }
    }

    return vect_l;
}

}// octopus
