#ifndef __DivinityModelLoader__
#define __DivinityModelLoader__

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

namespace fas
{

enum class DivinityType
{
    AttackSpeed,
    Economic,
    Heal,
    Lifesteal,
    Production,
    Recycle,
	None
};

/// @brief load the library with all divinity models
/// @param lib_p the library to fill
void loadLibrary(octopus::Library &lib_p);

std::vector<int> divinityTypeToInt(std::vector<DivinityType> const &types_p);
std::vector<DivinityType> intToDivinityType(std::vector<int> const &types_p);

std::vector<octopus::Steppable *> newPlayerBuilding(unsigned long player_p, DivinityType type_l, octopus::Library const &lib_p);

std::string divinityUpgradeName(DivinityType type_p);

DivinityType rollOneRandomDivinity(octopus::RandomGenerator &rand_p, std::vector<DivinityType> const &exceptions_p);
DivinityType rollOneRandomDivinityFromList(octopus::RandomGenerator &rand_p, std::vector<DivinityType> const &list_p, std::vector<DivinityType> const &exceptions_p);

std::vector<DivinityType> allDivinities();

/// @brief get all the divinities that are maxed out
/// @param player_p the player to check from
/// @param maxLevel_p the level to be considered maxed ou
/// @return the vector of maxed out divinities
std::vector<DivinityType> getMaxedOutDivinities(octopus::Player const &player_p, unsigned long maxLevel_p=3);

void addBuildingPlayer(std::list<octopus::Steppable *> &spawners_p, unsigned long player_p, std::vector<DivinityType> const &divinities_p, octopus::Library &lib_p);
void addDivinityBuildingPlayer(std::list<octopus::Steppable *> &spawners_p, unsigned long player_p, std::vector<DivinityType> const &divinities_p, octopus::Library &lib_p);

} // namespace fas

#endif
