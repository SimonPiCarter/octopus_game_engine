#ifndef __Player__
#define __Player__

#include <string>
#include <vector>
#include <list>
#include <map>
#include "state/ResourceType.hh"
#include "state/DivinityType.hh"

#include "DivinityOption.hh"
#include "StepOptionsGenerator.hh"

namespace octopus
{
	struct BuildingModel;
	struct UnitModel;

	class Player
	{
		public:
			~Player();

			std::map<ResourceType, double> _resources;

			unsigned long _popCap {0};
			unsigned long _pop {0};

			unsigned long _id {0};
			unsigned long _team {0};

			/// @brief Available building models
			std::list<BuildingModel const *> _buildingModels;

			///
			/// Divinity info
			///
			/// @brief the level for every divinity
			std::map<DivinityType, unsigned long> _divLvl;
			/// @brief the anchor for every divinity
			std::map<DivinityType, double> _divAnchor;
			/// @brief the options currently available to the player
			std::list<DivinityOption> _divOptions;

			/// @brief the map of options available to the player
			/// @note Player is owner of the generator here
			std::map<std::string, StepOptionsGenerator*> _options;

			///
			/// Unlock info
			///
			/// @brief keep track of how many buildings of the given model are standing for this player
			std::map<std::string, unsigned long> _buildingCount;
			/// @brief keep track if a research has been unlocked already
			std::map<std::string, bool> _techUnlocked;
	};

	/// @brief safe getter for resource of a player
	double getResource(Player const &player_p, ResourceType type_p);
	/// @brief safe getter for divinity option of a player
	bool getDivOption(DivinityOption const &option_p, DivinityType type_p);
	/// @brief safe getter for divinity level of a player
	unsigned long getDivLvl(Player const &player_p, DivinityType type_p);
	/// @brief safe getter for divinity anchor of a player
	/// @param floor_p if true the value will be floored to zero if negative
	double getDivAnchor(Player const &player_p, DivinityType type_p, bool floor_p=true);

	/// @brief return availables building models from building
	std::list<BuildingModel const *> getAvailableBuildingModels(Player const &player_p);
	/// @brief return availables unit models from building and player
	std::list<UnitModel const *> getAvailableUnitModels(BuildingModel const &building_p, Player const &player_p);
}

#endif
