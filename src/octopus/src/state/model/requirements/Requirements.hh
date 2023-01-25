#ifndef __Requirements__
#define __Requirements__


#include <map>
#include <set>
#include <string>

#include "state/DivinityType.hh"

namespace octopus
{
	class Player;

	/// @brief this class represent requirements for Building, Unit and technologies
	struct Requirements
	{
		std::set<std::string> _buildings;
		std::set<std::string> _technologies;
		std::map<DivinityType, unsigned long> _divLvl;
	};

	/// @brief check if a given player meet the given requirements
	bool meetRequirements(Requirements const &req_p, Player const &player_p);
} // namespace octopus


#endif
