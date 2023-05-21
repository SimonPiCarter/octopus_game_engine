#include "StepUpgradeTyppedBuffGenerator.hh"

#include "step/player/PlayerBuffAllStep.hh"

namespace octopus
{

StepUpgradeTyppedBuffGenerator::StepUpgradeTyppedBuffGenerator(TyppedBuff const &buff_p, std::vector<std::string> const &models_p)
    : _buff(buff_p), _models(models_p)
{}
StepUpgradeTyppedBuffGenerator::~StepUpgradeTyppedBuffGenerator()
{}

StepUpgradeGenerator* StepUpgradeTyppedBuffGenerator::newCopy() const
{
    return new StepUpgradeTyppedBuffGenerator(_buff, _models);
}

std::vector<Steppable *> StepUpgradeTyppedBuffGenerator::getSteppables(unsigned long player_p) const
{
    std::vector<Steppable *> vect_l;

    for(std::string const &model_l : _models)
    {
        vect_l.push_back(new PlayerBuffAllStep(player_p, _buff, model_l));
    }

    return vect_l;
}

}// octopus