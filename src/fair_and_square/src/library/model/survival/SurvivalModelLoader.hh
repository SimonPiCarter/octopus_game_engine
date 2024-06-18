#ifndef __Survival_ModelLoader__
#define __Survival_ModelLoader__

#include <list>
#include <vector>
#include <string>

namespace octopus
{
	class Library;
    class Player;
    class RandomGenerator;
    class Steppable;
} // namespace octopus

namespace octopus
{
    class Library;
} // namespace octopus

namespace fas
{

enum class SurvivalSpecialType
{
    AttackSpeed,
    Heal,
    Rebound,
    AreaOfDamage,
	None
};

void createOptionUpgrades(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, std::vector<fas::SurvivalSpecialType> const &forbidden_p);
void loadSurvivalModels(octopus::Library &lib_p);

std::vector<octopus::Steppable *> newPlayerBuilding(unsigned long player_p, SurvivalSpecialType type_p, octopus::Library const &lib_p);
std::string survivalSpecialTypeName(SurvivalSpecialType type_p);
SurvivalSpecialType rollOneRandomSurvivalSpecialType(octopus::RandomGenerator &rand_p, std::vector<SurvivalSpecialType> const &exceptions_p);
SurvivalSpecialType rollOneRandomSurvivalSpecialTypeFromList(octopus::RandomGenerator &rand_p, std::vector<SurvivalSpecialType> const &list_p, std::vector<SurvivalSpecialType> const &exceptions_p);
std::vector<SurvivalSpecialType> allSurvivalSpecialTypes();
std::vector<SurvivalSpecialType> notDemoSurvivalSpecialTypes();
std::vector<SurvivalSpecialType> demoSurvivalSpecialTypes();
std::vector<SurvivalSpecialType> getMaxedOutSurvivalSpecialTypes(octopus::Player const &player_p, unsigned long maxLevel_p=1);

void addSurvivalBuildingPlayer(std::list<octopus::Steppable *> &spawners_p, unsigned long player_p, octopus::Library &lib_p);

std::vector<std::string> allSurivvalModels();
}

#endif
