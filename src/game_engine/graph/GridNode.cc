#include "GridNode.hh"

namespace game_engine
{

GridNode::GridNode(Vector const &pos_p):  _position(pos_p)
{}

Vector const & GridNode::getPosition() const
{
	return _position;
}

void GridNode::setFree(bool free_p)
{
	_free = free_p;
}
bool GridNode::isFree() const
{
	return _free;
}

}
