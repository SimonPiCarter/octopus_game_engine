#ifndef __ScoreTracker__
#define __ScoreTracker__

#include <vector>

namespace octopus
{

// tracks per player :
// - resources harvested
// - unit lost
struct ScoreTracker
{
public:
	void add_harvested_resource(unsigned int player_p, unsigned long long resources_p);
	void add_unit_lost(unsigned int player_p);

	std::vector<unsigned long long> const &get_harvested_resources() const { return harvested_resources; }
	std::vector<unsigned long long> const &get_unit_losts() const { return unit_losts; }
private:
	std::vector<unsigned long long> harvested_resources;
	std::vector<unsigned long long> unit_losts;
};

}
#endif
