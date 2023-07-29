#ifndef __DivinityModelLoader__
#define __DivinityModelLoader__

#include <vector>
#include <string>

namespace octopus
{
    class Steppable;
	class Library;
    class RandomGenerator;
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
    Recycle
};

/// @brief load the library with all divinity models
/// @param lib_p the library to fill
void loadLibrary(octopus::Library &lib_p);

std::vector<int> divinityTypeToInt(std::vector<DivinityType> const &types_p);
std::vector<DivinityType> intToDivinityType(std::vector<int> const &types_p);

std::vector<octopus::Steppable *> newPlayerBuilding(unsigned long player_p, DivinityType type_l, octopus::Library const &lib_p);

std::string divinityUpgradeName(DivinityType type_p);

DivinityType rollOneRandomDivinity(octopus::RandomGenerator &rand_p);

} // namespace fas

#endif
