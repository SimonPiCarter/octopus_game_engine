#ifndef __RandomGen__
#define __RandomGen__

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace octopus
{

/// @brief this class is unique per game and allow consistent random generation using boost since std is not platform independant
class RandomGenerator
{
public:
	RandomGenerator(unsigned long seed_p) : _gen(seed_p) {}

	int roll(int min_p, int max_p)
	{
		boost::random::uniform_int_distribution<> distModel_l(min_p, max_p);
		return distModel_l(_gen);
	}

private:
	boost::random::mt19937 _gen;
};


/// @brief implement Xoshiro128+ from https://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
class RandomGen
{
public:
	RandomGen(unsigned long seed_p)
	{
		_shuffle_table[0] = seed_p;
		_shuffle_table[1] = seed_p*seed_p;
		_shuffle_table[2] = seed_p*13;
		_shuffle_table[3] = seed_p*53;
	}

	unsigned long next()
	{
		unsigned long s1 = _shuffle_table[0];
		unsigned long s0 = _shuffle_table[1];
		unsigned long result = s0 + s1;
		_shuffle_table[0] = s0;
		s1 ^= s1 << 23;
		_shuffle_table[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
		return result;
	}

	unsigned long nextFromRange(unsigned long min_p, unsigned long max_p)
	{
		unsigned long res_l = next();
		unsigned long range_l = max_p-min_p;
		res_l = res_l % range_l;
		return res_l + min_p;
	}

private:
	unsigned long _shuffle_table[4];
};
} // namespace octopus

#endif
