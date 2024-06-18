#include "ScoreTracker.hh"

namespace octopus
{

void ScoreTracker::add_harvested_resource(unsigned int player_p, unsigned long long resources_p)
{
	if(harvested_resources.size() <= player_p)
	{
		harvested_resources.resize(player_p+1, 0);
	}
	harvested_resources[player_p] += resources_p;
}

void ScoreTracker::add_unit_lost(unsigned int player_p)
{
	if(units_lost.size() <= player_p)
	{
		units_lost.resize(player_p+1, 0);
	}
	units_lost[player_p] += 1;
}

void ScoreTracker::add_unit_produced(unsigned int player_p)
{
	if(units_produced.size() <= player_p)
	{
		units_produced.resize(player_p+1, 0);
	}
	units_produced[player_p] += 1;
}

}
