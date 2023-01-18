#ifndef __GRID_NODE__
#define __GRID_NODE__

#include "utils/Vector.hh"

namespace octopus
{

class Entity;

/// @brief represent a node in the Grid
class GridNode
{
public:
	explicit GridNode(Vector const &pos_p);

	Vector const & getPosition() const;

	void setContent(Entity const *content_p);
	Entity const * getContent() const;

	void setFree(bool free_p);
	bool isFree() const;
private:
	Vector _position;
	bool _free {true};
	Entity const * _content {nullptr};
};

} // namespace octopus

#endif
