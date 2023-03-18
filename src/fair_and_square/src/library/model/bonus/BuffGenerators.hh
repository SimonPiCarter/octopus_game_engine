#ifndef __BuffGenerators__
#define __BuffGenerators__

#include <random>
#include <vector>

#include "state/player/StepOptionsGenerator.hh"
#include "state/entity/Buff.hh"

struct BuffOption
{
    octopus::TyppedBuff _buff;
    std::string _model;
};

class BuffGenerator : public octopus::StepOptionsGenerator
{
public:
    BuffGenerator(std::vector<BuffOption> const &options_p) : _options(options_p) {}
	~BuffGenerator();

    virtual StepOptionsGenerator* newCopy() const override { return new BuffGenerator(_options); }

    virtual std::vector<octopus::Steppable *> const &getSteppables(unsigned long options_p, unsigned long player_p) override;

    virtual unsigned long getNumOptions() const override { return _options.size(); }

    std::vector<BuffOption> const _options;
private:
	std::vector<octopus::Steppable *> _steps;
};

BuffOption generateRandomOption(std::mt19937 &_gen, std::string const &id_p);

#endif
