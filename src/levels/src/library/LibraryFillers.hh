#ifndef __LibraryFiller__
#define __LibraryFiller__

namespace octopus
{
	class Library;
} // namespace octopus


/// @brief fill library with info from swarming divinity (Divinity_1)
void divinitySwarmFiller(octopus::Library &lib_p);

/// @brief fill library with info from raiding divinity (Divinity_3)
void divinityRaidFiller(octopus::Library &lib_p);

/// @brief fill library with info from armored divinity (Divinity_4)
void divinityArmorFiller(octopus::Library &lib_p);

/// @brief fill library with info from rate of fire divinity (Divinity_5)
void divinityFireFiller(octopus::Library &lib_p);

#endif
