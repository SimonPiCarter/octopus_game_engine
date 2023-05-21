#include "Requirements.hh"

#include "state/player/Player.hh"

namespace octopus
{

bool meetRequirements(Requirements const &req_p, Player const &player_p)
{
	// check no buildings
	for(std::string const & id_l : req_p._noBuildings)
	{
		auto &&it_l = player_p._buildingCount.find(id_l);
		if(it_l != player_p._buildingCount.end() && it_l->second > 0l)
		{
			return false;
		}
	}

	// check buildings
	for(std::string const & id_l : req_p._buildings)
	{
		auto &&it_l = player_p._buildingCount.find(id_l);
		if(it_l == player_p._buildingCount.end() || it_l->second <= 0l)
		{
			return false;
		}
	}

	// check upgrade lvl min
	for(auto &&pair_l : req_p._upgradeLvl)
	{
		std::string type_l = pair_l.first;
		unsigned long lvl_l = pair_l.second;
		auto &&it_l = player_p._upgradeLvl.find(type_l);
		if(it_l == player_p._upgradeLvl.end() || it_l->second < lvl_l)
		{
			return false;
		}
	}

	// check upgrade lvl max
	for(auto &&pair_l : req_p._upgradeLvlMax)
	{
		std::string type_l = pair_l.first;
		unsigned long lvl_l = pair_l.second;
		auto &&it_l = player_p._upgradeLvl.find(type_l);
		if(it_l == player_p._upgradeLvl.end() || it_l->second > lvl_l)
		{
			return false;
		}
	}

	return true;
}

} // namespace octopus
