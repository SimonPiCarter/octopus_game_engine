#ifndef __AnchorTriggerOptionGenerator__
#define __AnchorTriggerOptionGenerator__

#include "library/model/bonus/BuffGenerators.hh"

#include <functional>
#include <list>

namespace octopus
{
	class State;
	class RandomGenerator;
} // namespace octopus


Option no_op(octopus::State const &, unsigned long, octopus::RandomGenerator &, std::string const &);

struct AnchorOptionGenerator
{
	unsigned long proba_coef = 10;
	std::function<Option(octopus::State const &, unsigned long, octopus::RandomGenerator &, std::string const &)> generator = no_op;
	bool allow_multiple = false;
};

std::list<AnchorOptionGenerator> generateOptionGenerators(octopus::State const &, unsigned long);

unsigned long getSumOfProba(std::list<AnchorOptionGenerator> const &generators_p);

#endif
