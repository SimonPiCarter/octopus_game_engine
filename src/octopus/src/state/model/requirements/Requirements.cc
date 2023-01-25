#include "Requirements.hh"

#include "state/player/Player.hh"

namespace octopus
{

bool meetRequirements(Requirements const &req_p, Player const &player_p)
{
	// check buildings
	for(std::string const & id_l : req_p._buildings)
	{
		auto &&it_l = player_p._buildingCount.find(id_l);
		if(it_l == player_p._buildingCount.end() || it_l->second == 0ul)
		{
			return false;
		}
	}

	// check techs
	for(std::string const & id_l : req_p._technologies)
	{
		auto &&it_l = player_p._techUnlocked.find(id_l);
		if(it_l == player_p._techUnlocked.end() || !it_l->second)
		{
			return false;
		}
	}

	// check divinity lvl
	for(auto &&pair_l : req_p._divLvl)
	{
		DivinityType type_l = pair_l.first;
		unsigned long lvl_l = pair_l.second;
		auto &&it_l = player_p._divLvl.find(type_l);
		if(it_l == player_p._divLvl.end() || it_l->second < lvl_l)
		{
			return false;
		}
	}

	return true;
}

} // namespace octopus
