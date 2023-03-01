#include "CanonicalDijkstra.hh"

#include <list>
#include <set>
#include <cassert>

namespace octopus
{

bool isCanonical(ValueNode const &child_p, ValueNode const &parent_p)
{
	return child_p.y == parent_p.y;
}

bool isCardinal(ValueNode const &child_p, ValueNode const &parent_p)
{
	return !isCanonical(child_p, parent_p);
}

bool isJumpPoint(ValueNode const &child_p, ValueNode const &parent_p, std::vector<std::vector<GridNode *> > const &grid_p)
{
	if(isCanonical(child_p, parent_p))
	{
		return false;
	}

	if(parent_p.x < grid_p.size() - 1
	&& child_p.x < grid_p.size() - 1
	&& !grid_p[parent_p.x+1][parent_p.y]->isFree()
	&& grid_p[child_p.x+1][child_p.y]->isFree())
	{
		return true;
	}
	if(parent_p.x > 0
	&& child_p.x > 0
	&& !grid_p[parent_p.x-1][parent_p.y]->isFree()
	&& grid_p[child_p.x-1][child_p.y]->isFree())
	{
		return true;
	}
	return false;
}

ValueNode apply(ValueNode const &child_p, ValueNode const &parent_p, std::vector<std::vector<GridNode *> > const &grid_p)
{
	long dx = long(child_p.x) - long(parent_p.x);
	long dy = long(child_p.y) - long(parent_p.y);
	return ValueNode {child_p.x + dx, child_p.y + dy, 0.};
}

ValueNode apply_first_cardinal(ValueNode const &child_p, ValueNode const &parent_p, std::vector<std::vector<GridNode *> > const &grid_p)
{
	return ValueNode {child_p.x, child_p.y + 1, 0.};
}

ValueNode apply_second_cardinal(ValueNode const &child_p, ValueNode const &parent_p, std::vector<std::vector<GridNode *> > const &grid_p)
{
	return ValueNode {child_p.x, child_p.y - 1, 0.};
}

struct canonical_entries
{
	canonical_entries(ValueNode const &child_p, ValueNode const &parent_p, Fixed const &cost_p)
		: child(child_p), parent(parent_p), cost(cost_p) {}
	ValueNode child;
	ValueNode parent;
	Fixed cost;
};

void canonical_ordering(ValueNode const &child_p, ValueNode const &parent_p, Fixed cost_p,
	ValueGrid &closed_p, std::set<ValueNode, ValueSorter> &open_p, std::vector<std::vector<GridNode *> > const &grid_p,
	std::list<canonical_entries> &entries_p)
{
	// skip out of bound
	if(child_p.x < 0 || child_p.y < 0 || child_p.x >= grid_p.size() || child_p.y >= grid_p[child_p.x].size())
	{
		return;
	}

	// skip if not free
	if(!grid_p[child_p.x][child_p.y]->isFree())
	{
		return;
	}

	// if better we update
	if(cost_p < closed_p[child_p.x][child_p.y]
	|| closed_p[child_p.x][child_p.y] < 0.)
	{
		closed_p[child_p.x][child_p.y] = cost_p;
	}
	// else just stop
	else
	{
		return;
	}

	/// special case when starting over
	if(child_p == parent_p)
	{
		ValueNode next_l = child_p;
		next_l.x += 1;
		entries_p.emplace_front(next_l, child_p, cost_p + 1);

		next_l = child_p;
		next_l.x -= 1;
		entries_p.emplace_front(next_l, child_p, cost_p + 1);

		next_l = child_p;
		next_l.y += 1;
		entries_p.emplace_front(next_l, child_p, cost_p + 1);

		next_l = child_p;
		next_l.y -= 1;
		entries_p.emplace_front(next_l, child_p, cost_p + 1);
	}
	else if(isJumpPoint(child_p, parent_p, grid_p))
	{
		closed_p[child_p.x][child_p.y] = -1;
		ValueNode new_l = child_p;
		new_l.cost = cost_p;
		open_p.insert(new_l);
	}
	else if(isCanonical(child_p, parent_p))
	{
		// apply the cardinal
		ValueNode next_l = apply(child_p, parent_p, grid_p);
		entries_p.emplace_front(next_l, child_p, cost_p + 1);
		// apply first cardinal
		next_l = apply_first_cardinal(child_p, parent_p, grid_p);
		entries_p.emplace_front(next_l, child_p, cost_p + 1.);
		// apply second cardinal
		next_l = apply_second_cardinal(child_p, parent_p, grid_p);
		entries_p.emplace_front(next_l, child_p, cost_p + 1.);
	}
	else if(isCardinal(child_p, parent_p))
	{
		// apply the cardinal
		ValueNode next_l = apply(child_p, parent_p, grid_p);
		entries_p.emplace_front(next_l, child_p, cost_p + 1);
	}
}

ValueGrid canonical_dijkstra(std::vector<std::vector<GridNode *> > const &grid_p, long x, long y)
{
	std::set<ValueNode, ValueSorter> open_l;
	ValueGrid grid_l;
	/// init all node in closed with -1 (to mean +infinity)
	for(long i = 0 ; i < grid_p.size() ; ++i)
	{
		grid_l.emplace_back(grid_p[i].size(), Fixed(-1.));
	}

	open_l.insert(ValueNode{x, y, 0.});

	/// @brief all entries for every node opened
	std::list<canonical_entries> entries_l;

	while(!open_l.empty())
	{
		ValueNode best_l = *open_l.begin();
		open_l.erase(open_l.begin());
		canonical_ordering(best_l, best_l, best_l.cost, grid_l, open_l, grid_p, entries_l);
		while(!entries_l.empty())
		{
			canonical_entries entry_l = entries_l.front();
			entries_l.pop_front();
			canonical_ordering(entry_l.child, entry_l.parent, entry_l.cost, grid_l, open_l, grid_p, entries_l);
		}
	}

	return grid_l;
}

Vector compute_field(ValueGrid const &grid_p, long x, long y)
{
	Fixed dx = 0;
	Fixed dy = 0;
	Fixed best_l(-1);
	for(long i = -1 ; i < 2 ; ++i)
	{
		// skip out of bound
		if(x+i < 0 || x+i >= grid_p.size())
		{
			continue;
		}
		for(long j = -1 ; j < 2 ; ++j)
		{
			// skip out of bound
			if(y+j < 0 || y+j >= grid_p[x+i].size())
			{
				continue;
			}
			if((best_l < 0.
			|| best_l > grid_p[x+i][y+j])
			&& grid_p[x+i][y+j] >= 0.)
			{
				dx = i;
				dy = j;
				best_l = grid_p[x+i][y+j];
			}
		}
	}
	return {dx, dy};
}

FlowField flow_field(ValueGrid const &grid_p)
{
	FlowField result_l;
	for(long i = 0 ; i < grid_p.size() ; ++i)
	{
		result_l.emplace_back(grid_p[i].size(), Vector{0., 0.});
		for(long j = 0 ; j < grid_p[i].size() ; ++j)
		{
			if(grid_p[i][j] >= 0)
			{
				result_l[i][j] = compute_field(grid_p, i, j);
			}
		}
	}
	return result_l;
}

std::ostream &stream(std::ostream & os_p, ValueGrid const &grid_p)
{
	for(size_t i = 0; i < grid_p.size() ; ++ i)
	{
		for(size_t j = 0; j < grid_p.size() ; ++ j)
		{
			assert(grid_p[j].size() > i);
			os_p<<grid_p[j][i]<<",\t";
		}
		os_p<<std::endl;
	}
	return os_p;
}

std::ostream &stream(std::ostream & os_p, FlowField const &field_p)
{
	for(size_t i = 0; i < field_p.size() ; ++ i)
	{
		for(size_t j = 0; j < field_p.size() ; ++ j)
		{
			assert(field_p[j].size() > i);
			os_p<<field_p[j][i]<<",\t";
		}
		os_p<<std::endl;
	}
	return os_p;
}

}
