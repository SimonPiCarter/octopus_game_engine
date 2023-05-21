#ifndef __Requirements__
#define __Requirements__


#include <map>
#include <set>
#include <string>


namespace octopus
{
	class Player;

	/// @brief this class represent requirements for Building, Unit and technologies
	struct Requirements
	{
		std::set<std::string> _noBuildings;
		std::set<std::string> _buildings;
		std::map<std::string, unsigned long> _upgradeLvl;
	};

	/// @brief check if a given player meet the given requirements
	bool meetRequirements(Requirements const &req_p, Player const &player_p);
} // namespace octopus


#endif
