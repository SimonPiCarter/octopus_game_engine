#include "GridNode.hh"

namespace octopus
{

GridNode::GridNode(Vector const &pos_p):  _position(pos_p)
{}

Vector const & GridNode::getPosition() const
{
	return _position;
}

void GridNode::setContent(Entity const *content_p)
{
	_content = content_p;
}

Entity const * GridNode::getContent() const
{
	return _content;
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
