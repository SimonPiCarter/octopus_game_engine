#ifndef __VisionGrid__
#define __VisionGrid__

#include <vector>

namespace octopus
{
class Entity;

/// @brief this class is spposed to handle
/// vision
/// it only handles one vision (for one team usually)
class VisionGrid
{
public:
	VisionGrid(unsigned long size_p);

	/// @brief check if the given node is visible
	/// @warning no sanity check if performed, it assumes x and y are
	/// < than size_p given in constructor
	bool isVisible(unsigned long x, unsigned long y) const;

	/// @brief update the grid vision count
	/// @param ent_p the entity used to update
	/// @param set_p true if the entity should be added to the vision grid
	/// false if it should be removed
	/// @warning do not check team of entity
	void updateVision(const Entity &ent_p, bool set_p);

protected:
	/// @brief this is the vision grid
	/// it tracks the number of unit seeing one node of the grid
	std::vector<std::vector<long> > _grid;
};

} // octopus

#endif
