#include "Randomizer.hh"

#include "utils/RandomGenerator.hh"

using namespace octopus;

namespace fas
{

std::string genModelName(octopus::RandomGenerator &gen_p)
{
	std::string model_l = "square";
	int random_l = gen_p.roll(1, 2);
	if(random_l==1)
	{
		model_l = "triangle";
	}
	else if(random_l==2)
	{
		model_l = "circle";
	}
	return model_l;
}

} // fas
