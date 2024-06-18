#include "WaveBuffGenerator.hh"

#include "library/model/divinity/generators/UnitModelIds.hh"
#include "library/model/survival/SurvivalModelLoader.hh"
#include "library/model/survival/SurvivalNameGenerator.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

template < octopus::TyppedBuff::Type type_t, int base >
octopus::TimedBuff flat_basic(unsigned long waveCount_p, unsigned long upIdx_p)
{
	octopus::TimedBuff tBuff_l;
	tBuff_l._id = "wave_buff.flat_basic."+std::to_string(waveCount_p)+(upIdx_p > 0? std::to_string(upIdx_p) : "" );
	tBuff_l._type = type_t;
	tBuff_l._offset = base * waveCount_p;

	return tBuff_l;
}

std::list<WaveBuffGenerator> generateWaveBuffGenerators(unsigned long waveCount_p, unsigned long upIdx_p)
{
	std::list<WaveBuffGenerator> generators_l {
		// Common
			// Flat basic
			{ 10, flat_basic<octopus::TyppedBuff::Type::Damage, 2>(waveCount_p, upIdx_p), CircleGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::Damage, 2>(waveCount_p, upIdx_p), TriangleGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::HpMax, 10>(waveCount_p, upIdx_p), CircleGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::HpMax, 10>(waveCount_p, upIdx_p), TriangleGen::gen(), false },
		// Special
			{ 10, flat_basic<octopus::TyppedBuff::Type::Damage, 2>(waveCount_p, upIdx_p), USquareGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::Damage, 2>(waveCount_p, upIdx_p), QuadSquareGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::Damage, 2>(waveCount_p, upIdx_p), DoubleSquareGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::HpMax, 10>(waveCount_p, upIdx_p), USquareGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::HpMax, 10>(waveCount_p, upIdx_p), QuadSquareGen::gen(), false },
			{ 10, flat_basic<octopus::TyppedBuff::Type::HpMax, 10>(waveCount_p, upIdx_p), DoubleSquareGen::gen(), false },
	};

	return generators_l;
}

unsigned long getSumOfProba(std::list<WaveBuffGenerator> const &generators_p)
{
	unsigned long proba_l = 0;
	for(WaveBuffGenerator const &gen_l : generators_p)
	{
		proba_l += gen_l.proba_coef;
	}
	return proba_l;
}

std::vector<WaveBuffGenerator> getWaveBuffGenerator(octopus::RandomGenerator &rand_p, std::list<WaveBuffGenerator> generators_p, size_t nb_p)
{
	std::vector<WaveBuffGenerator> generators_l;
	size_t nb_rolls_l = nb_p;
	// for each roll
	for(size_t roll_l = 0 ; roll_l < nb_rolls_l ; ++ roll_l)
	{
		// get the sum of all options coef in the pool
		unsigned long sum_proba_l = getSumOfProba(generators_p);
		// roll random number between 0 and sum of
		unsigned long rolled_number_l = rand_p.roll(0, int(sum_proba_l));
		// set up threshold = 0
		unsigned long threshold_l = 0;
		// iterate on all option generator in the pool
		for(auto it_l = generators_p.begin() ; it_l != generators_p.end() ; ++it_l)
		{
			WaveBuffGenerator const &generator_l = *it_l;
			// increment threshold with the current proba coef
			threshold_l += generator_l.proba_coef;
			// if rolled number < threshold
			if(rolled_number_l <= threshold_l)
			{
				// generate option
				generators_l.push_back(generator_l);
				// if current option does not allow multiple
				if(!generator_l.allow_multiple)
				{
					// clear option generator from pool
					generators_p.erase(it_l);
				}
				// break inner loop
				break;
			}

		}
	}

	return generators_l;
}
