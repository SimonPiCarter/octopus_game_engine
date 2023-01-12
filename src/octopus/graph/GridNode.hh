#ifndef __GRID_NODE__
#define __GRID_NODE__

#include "entity/Vector.hh"

namespace octopus
{

/// @brief represent a node in the Grid
class GridNode
{
public:
	explicit GridNode(Vector const &pos_p);

	Vector const & getPosition() const;

	void setFree(bool free_p);
	bool isFree() const;
private:
	Vector _position;
	bool _free {true};
};

} // namespace octopus

#endif
