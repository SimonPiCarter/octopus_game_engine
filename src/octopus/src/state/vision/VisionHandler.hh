#ifndef __VisionHandler__
#define __VisionHandler__

#include <vector>

namespace octopus
{
class Entity;
class State;
class VisionGrid;

/// @brief This class manages vision for
/// multiple teams
class VisionHandler
{
public:
	VisionHandler(unsigned long size_p);
	~VisionHandler();

	/// @brief check if the given enity is visible for a given team
	bool isVisible(unsigned long team_p, const Entity &ent_p) const;

	/// @brief check if the given node is visible for a given team
	bool isVisible(unsigned long team_p, unsigned long x, unsigned long y) const;

	/// @brief check if the given node is explored for a given team
	bool isExplored(unsigned long team_p, unsigned long x, unsigned long y) const;

	/// @brief update the grid vision
	/// @param ent_p the entity used to update
	/// @param set_p true if the entity should be added to the vision grid
	/// false if it should be removed
	void updateVision(const State &state_p, const Entity &ent_p, bool set_p);

	/// @brief update the global grid vision
	/// @param ent_p the entity used to update
	/// @param set_p true if the entity should be added to the vision grid
	/// false if it should be removed
	void updateExploration(const State &state_p, const Entity &ent_p, bool set_p);

	/// @brief update the grid vision from movement
	/// @param ent_p the entity used to update
	void updateVisionFromMovement(const State &state_p, const Entity &ent_p, long dx, long dy);

	/// @brief update the global grid vision from movement
	/// @param ent_p the entity used to update
	void updateExplorationFromMovement(const State &state_p, const Entity &ent_p, long dx, long dy);

protected:
	unsigned long const _size;
	/// @brief vision grid for every team
	std::vector<VisionGrid *> _grid;

	VisionGrid * safeGetGrid(const State &state_p, unsigned long player_p);
};

} // octopus

#endif
