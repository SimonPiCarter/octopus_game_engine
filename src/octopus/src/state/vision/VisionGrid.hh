#ifndef __VisionGrid__
#define __VisionGrid__

#include <cstddef>
#include <vector>

namespace octopus
{
class Entity;
class PatternHandler;

/// @brief this class is spposed to handle
/// vision
/// it only handles one vision (for one team usually)
class VisionGrid
{
public:
	VisionGrid(unsigned long size_p);

	/// @brief check if the given entity is visible
	bool isVisible(const Entity &ent_p) const;

	/// @brief check if the given entity is explored
	bool isExplored(const Entity &ent_p) const;

	/// @brief check if the given node is visible
	/// @warning no sanity check if performed, it assumes x and y are
	/// < than size_p given in constructor
	bool isVisible(unsigned long x, unsigned long y) const;

	/// @brief check if the given node is explored
	/// @warning no sanity check if performed, it assumes x and y are
	/// < than size_p given in constructor
	bool isExplored(unsigned long x, unsigned long y) const;

	/// @brief update the grid vision count
	/// @param ent_p the entity used to update
	/// @param set_p true if the entity should be added to the vision grid
	/// false if it should be removed
	/// @warning do not check team of entity
	void updateVision(const Entity &ent_p, bool set_p, PatternHandler &patternHandler_p);

	/// @brief update the grid exploration count
	/// @param ent_p the entity used to update
	/// @param set_p true if the entity should be added to the vision grid
	/// false if it should be removed
	/// @warning do not check team of entity
	void updateExploration(const Entity &ent_p, bool set_p, PatternHandler &patternHandler_p);

	/// @brief update the grid vision count from a movement in the grid
	/// @param ent_p the entity used to update
	/// @warning do not check team of entity
	void updateVisionFromMovement(const Entity &ent_p, long dx, long dy, PatternHandler &patternHandler_p);

	/// @brief update the grid exploration count from a movement in the grid
	/// @param ent_p the entity used to update
	/// @warning do not check team of entity
	void updateExplorationFromMovement(const Entity &ent_p, long dx, long dy, PatternHandler &patternHandler_p);

	/// @brief increment given grid element
	void incrementVisionGrid(size_t x, size_t y, long long delta_p, bool exploration_p);

	unsigned long const &getSize() const { return _size; }
protected:
	/// @brief size of the grid
	unsigned long const _size;
	/// @brief this is the vision grid
	/// it tracks the number of unit seeing one node of the grid
	std::vector<std::vector<long long> > _grid;

	/// @brief track if a node has been explored already
	std::vector<std::vector<long long> > _exploration;
};

} // octopus

#endif
