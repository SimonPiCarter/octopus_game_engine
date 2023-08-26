#include "RandomGenerator.hh"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

namespace octopus
{

/// @brief This is a basic implementation using boost
class BoostRandomGenertorStateful : public AbstractRandomGenertorStateful
{
public:
	BoostRandomGenertorStateful(unsigned long seed_p) : _gen(seed_p) {}

	int roll(int min_p, int max_p) override
	{
		boost::random::uniform_int_distribution<> distModel_l(min_p, max_p);
		return distModel_l(_gen);
	}
	double roll_double(double min_p, double max_p) override
	{
		boost::random::uniform_real_distribution<> distModel_l(min_p, max_p);
		return distModel_l(_gen);
	}

private:
	boost::random::mt19937 _gen;
};

RandomGenerator::RandomGenerator(unsigned long seed_p, bool alwaysMin_p) : _state(new BoostRandomGenertorStateful(seed_p)), _alwaysMin(alwaysMin_p) {}

RandomGenerator::~RandomGenerator()
{
	delete _state;
}

int RandomGenerator::roll(int min_p, int max_p)
{
	if(_alwaysMin)
	{
		return min_p;
	}
	return _state->roll(min_p, max_p);
}

double RandomGenerator::roll_double(double min_p, double max_p)
{
	if(_alwaysMin)
	{
		return min_p;
	}
	return _state->roll_double(min_p, max_p);
}

} // octopus