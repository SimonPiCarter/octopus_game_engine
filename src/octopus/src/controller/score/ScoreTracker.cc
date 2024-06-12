#include "ScoreTracker.hh"

namespace octopus
{

void ScoreTracker::add_harvested_resource(unsigned int player_p, unsigned long long resources_p)
{
	harvested_resources.resize(player_p+1, 0);
	harvested_resources[player_p] += resources_p;
}

void ScoreTracker::add_unit_lost(unsigned int player_p)
{
	unit_losts.resize(player_p+1, 0);
	unit_losts[player_p] += 1;
}

}
