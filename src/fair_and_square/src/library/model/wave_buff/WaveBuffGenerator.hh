#ifndef __WaveBuffGenerator__
#define __WaveBuffGenerator__

#include "library/model/bonus/BuffGenerators.hh"

#include <functional>
#include <list>
#include <vector>

namespace octopus
{
	class RandomGenerator;
	class TimedBuff;
} // namespace octopus

struct WaveBuffGenerator
{
	unsigned long proba_coef = 10;
	octopus::TimedBuff buff;
	std::string model;
	bool allow_multiple = false;
};

std::list<WaveBuffGenerator> generateWaveBuffGenerators(unsigned long waveCount_p);

unsigned long getSumOfProba(std::list<WaveBuffGenerator> const &generators_p);

std::vector<WaveBuffGenerator> getWaveBuffGenerator(octopus::RandomGenerator &rand_p, std::list<WaveBuffGenerator> generators_p, size_t nb_p);

#endif
