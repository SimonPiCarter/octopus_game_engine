#ifndef __Player__
#define __Player__

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include "state/entity/buff/TimedBuff.hh"
#include "state/entity/buff/ConditionalBuff.hh"
#include "state/entity/attackModifier/AttackModifier.hh"

#include "DivinityOption.hh"
#include "StepOptionsGenerator.hh"
#include "bonus/PlayerBonus.hh"

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

			/// @brief the list of options available to the player
			/// @note Player is owner of the generator here
			std::list<StepOptionsGenerator*> _options;

			/// @brief the list of all buffs that should be applied on every entity per model
			std::map<std::string, std::vector<TimedBuff> > _mapBuffs;

			/// @brief the list of all conditional buffs that should be applied on every entity per model
			std::map<std::string, std::vector<ConditionalBuff> > _mapConditionalBuffs;

			/// @brief the list of modifiers that should be applied on every entity per model
			std::map<std::string, AttackModifier > _mapModifiers;

			/// @brief sub container for all bonuses (production time and cost reduction for units ONLY)
			PlayerBonus _bonuses;

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
	/// @brief get the production time for the given model taking bonuses into account
	Fixed getProductionTime(UnitModel const &model_p, Player const & player_p);
	/// @brief get the cost of the given model taking bonuses into account
	std::map<std::string, Fixed> getCost(UnitModel const &model_p, Player const & player_p);

	/// @brief return availables building models from building
	std::list<BuildingModel const *> getAvailableBuildingModels(Player const &player_p, bool checkRequirements_p=true);
	/// @brief return availables unit models from building and player
	std::list<UnitModel const *> getAvailableUnitModels(BuildingModel const &building_p, Player const &player_p, bool checkRequirements_p=true);
	/// @brief Check if the player meet upgrade requirements
	bool checkUpgradeRequirements(Player const &player_p, Upgrade const &upgrade_p);
	/// @brief return availables upgrades from building and player
	std::list<Upgrade const *> getAvailableUpgrades(BuildingModel const &building_p, Player const &player_p, bool checkRequirements_p=true);
}

#endif
