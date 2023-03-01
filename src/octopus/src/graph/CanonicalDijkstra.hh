#ifndef __CanonicalDijkstra__
#define __CanonicalDijkstra__

#include <list>
#include <set>
#include <vector>

#include "GridNode.hh"
#include "utils/Fixed.hh"
#include "utils/Vector.hh"

namespace octopus
{

/// @brief represent a node in the search
struct ValueNode
{
	long x {0};
	long y {0};
	Fixed cost {0.};
	//GridNode const * node {nullptr};
	//bool open {false};

	bool operator<(ValueNode const &b) const
	{
		if(x == b.x)
		{
			return y < b.y;
		}
		return x < b.x;
	}

	bool operator==(ValueNode const &b) const
	{
		return x == b.x
			&& y == b.y
			&& cost == b.cost;
	}
};

struct ValueSorter
{
	bool operator()(ValueNode const &a, ValueNode const &b) const
	{
		if(a.cost == b.cost)
		{
			return a < b;
		}
		return a.cost < b.cost;
	}
};

struct canonical_entries
{
	canonical_entries(ValueNode const &child_p, ValueNode const &parent_p, Fixed const &cost_p)
		: child(child_p), parent(parent_p), cost(cost_p) {}
	ValueNode child;
	ValueNode parent;
	Fixed cost;
};

/// @brief a result of the shortest path with the value for each node
typedef std::vector<std::vector<Fixed> > ValueGrid;

/// @brief compute a value grid to access the node in position x and y
ValueGrid canonical_dijkstra(std::vector<std::vector<GridNode *> > const &grid_p, long x, long y);

/// @brief store a flow field
typedef std::vector<std::vector<Vector> > FlowField;

/// @brief compute a flow field based on a ValueGrid
FlowField flow_field(ValueGrid const &grid_p);

/// @brief comptute direction based on flow field by adding all vectors
Vector direction(Fixed x, Fixed y, FlowField &field_p);

std::ostream &stream(std::ostream & os_p, ValueGrid const &grid_p);
std::ostream &stream(std::ostream & os_p, FlowField const &field_p);

struct FlowFieldComputation
{
	FlowFieldComputation(std::vector<std::vector<GridNode *> > const &grid_p, long x_p, long y_p);

	/// @brief compute some iterations for flow field
	/// @return the number of remaining iteration if finished
	long long compute(long long iter_p);

	/// @return true if the flow field has been computed
	bool over() const;

	std::vector<std::vector<GridNode *> > const &grid;
	long const x {0};
	long const  y {0};
	FlowField result;
	ValueGrid valueGrid;

	/// @brief entries to track down progress
	std::list<canonical_entries> entries;
	std::set<ValueNode, ValueSorter> open;
};

} // namespace octopus



#endif
