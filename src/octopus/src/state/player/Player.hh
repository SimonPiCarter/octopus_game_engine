#ifndef __Player__
#define __Player__

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include "state/entity/Buff.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

#include "DivinityOption.hh"
#include "StepOptionsGenerator.hh"

namespace octopus
{
	struct BuildingModel;
	struct UnitModel;
	struct Upgrade;

	class Player
	{
		public:
			~Player();

			std::map<std::string, Fixed> _resources;

			unsigned long _id {0};
			unsigned long _team {0};

			/// @brief Available building models
			std::list<BuildingModel const *> _buildingModels;

			/// @brief the map of options available to the player
			/// @note Player is owner of the generator here
			std::map<std::string, StepOptionsGenerator*> _options;

			/// @brief the list of all buffs that should be applied on every entity per model
			std::map<std::string, std::vector<TyppedBuff> > _mapBuffs;

			/// @brief the list of modifiers that should be applied on every entity per model
			std::map<std::string, AttackModifier > _mapModifiers;

			///
			/// Unlock info
			///
			/// @brief keep track of how many buildings of the given model are standing for this player
			std::map<std::string, long> _buildingCount;

			/// @brief the level for every upgrade
			std::map<std::string, unsigned long> _upgradeLvl;

			/// @brief indicates if an upgrade is currently produced
			/// @note this is useful to avoid the same upgrade to be started multiple time
			/// @note an upgrade can be tagged repeatable to disregard this constraint
			std::unordered_set<std::string> _producedUpgrade;
	};

	/// @brief safe getter for resource of a player
	Fixed getResource(Player const &player_p, std::string const &type_p);
	/// @brief safe getter for divinity option of a player
	bool getDivOption(DivinityOption const &option_p, std::string const  &type_p);
	/// @brief safe getter for divinity level of a player
	unsigned long getUpgradeLvl(Player const &player_p, std::string const  &type_p);

	/// @brief return availables building models from building
	std::list<BuildingModel const *> getAvailableBuildingModels(Player const &player_p);
	/// @brief return availables unit models from building and player
	std::list<UnitModel const *> getAvailableUnitModels(BuildingModel const &building_p, Player const &player_p);
	/// @brief return availables upgrades from building and player
	std::list<Upgrade const *> getAvailableUpgrades(BuildingModel const &building_p, Player const &player_p);
}

#endif
