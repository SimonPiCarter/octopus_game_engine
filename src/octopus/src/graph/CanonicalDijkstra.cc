#include "CanonicalDijkstra.hh"

#include <set>

namespace octopus
{

bool isCardinal(ValueNode const &child_p, ValueNode const &parent_p)
{
	return child_p.x == parent_p.x || child_p.y == parent_p.y;
}

bool isDiagonal(ValueNode const &child_p, ValueNode const &parent_p)
{
	return !isCardinal(child_p, parent_p);
}

bool isJumpPoint(ValueNode const &child_p, ValueNode const &parent_p, std::vector<std::vector<GridNode *> > const &grid_p)
{
	if(isDiagonal(child_p, parent_p))
	{
		return false;
	}

	long dx = long(child_p.x) - long(parent_p.x);
	if(dx == 0)
	{
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
	}
	// dy == 0
	else
	{
		if(parent_p.y < grid_p[parent_p.x].size() - 1
		&& child_p.y < grid_p[child_p.x].size() - 1
		&& !grid_p[parent_p.x][parent_p.y+1]->isFree()
		&& grid_p[child_p.x][child_p.y+1]->isFree())
		{
			return true;
		}
		if(parent_p.y > 0
		&& child_p.y > 0
		&& !grid_p[parent_p.x][parent_p.y-1]->isFree()
		&& grid_p[child_p.x][child_p.y-1]->isFree())
		{
			return true;
		}
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
	long dx = long(child_p.x) - long(parent_p.x);
	return ValueNode {child_p.x + dx, child_p.y, 0.};
}

ValueNode apply_second_cardinal(ValueNode const &child_p, ValueNode const &parent_p, std::vector<std::vector<GridNode *> > const &grid_p)
{
	long dy = long(child_p.y) - long(parent_p.y);
	return ValueNode {child_p.x, child_p.y + dy, 0.};
}

void canonical_ordering(ValueNode const &child_p, ValueNode const &parent_p, Fixed cost_p,
	ValueGrid &closed_p, std::set<ValueNode, ValueSorter> &open_p, std::vector<std::vector<GridNode *> > const &grid_p)
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
		canonical_ordering(next_l, child_p, cost_p + 1, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.x -= 1;
		canonical_ordering(next_l, child_p, cost_p + 1, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.y += 1;
		canonical_ordering(next_l, child_p, cost_p + 1, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.y -= 1;
		canonical_ordering(next_l, child_p, cost_p + 1, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.x += 1;
		next_l.y += 1;
		canonical_ordering(next_l, child_p, cost_p + 1.5, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.x -= 1;
		next_l.y += 1;
		canonical_ordering(next_l, child_p, cost_p + 1.5, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.x += 1;
		next_l.y -= 1;
		canonical_ordering(next_l, child_p, cost_p + 1.5, closed_p, open_p, grid_p);

		next_l = child_p;
		next_l.x -= 1;
		next_l.y -= 1;
		canonical_ordering(next_l, child_p, cost_p + 1.5, closed_p, open_p, grid_p);
	}
	else if(isJumpPoint(child_p, parent_p, grid_p))
	{
		closed_p[child_p.x][child_p.y] = -1;
		ValueNode new_l = child_p;
		new_l.cost = cost_p;
		open_p.insert(new_l);
	}
	else if(isDiagonal(child_p, parent_p))
	{
		// apply the diagonal
		ValueNode next_l = apply(child_p, parent_p, grid_p);
		canonical_ordering(next_l, child_p, cost_p + 1.5, closed_p, open_p, grid_p);
		// apply first cardinal
		next_l = apply_first_cardinal(child_p, parent_p, grid_p);
		canonical_ordering(next_l, child_p, cost_p + 1., closed_p, open_p, grid_p);
		// apply second cardinal
		next_l = apply_second_cardinal(child_p, parent_p, grid_p);
		canonical_ordering(next_l, child_p, cost_p + 1., closed_p, open_p, grid_p);
	}
	else if(isCardinal(child_p, parent_p))
	{
		// apply the cardinal
		ValueNode next_l = apply(child_p, parent_p, grid_p);
		canonical_ordering(next_l, child_p, cost_p + 1, closed_p, open_p, grid_p);
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

	while(!open_l.empty())
	{
		ValueNode best_l = *open_l.begin();
		open_l.erase(open_l.begin());
		canonical_ordering(best_l, best_l, best_l.cost, grid_l, open_l, grid_p);
	}

	return grid_l;
}

std::ostream &stream(std::ostream & os_p, ValueGrid const &grid_p)
{
	for(size_t i = 0; i < grid_p.size() ; ++ i)
	{
		for(size_t j = 0; j < grid_p.size() ; ++ j)
		{
			os_p<<grid_p[j][i]<<",\t";
		}
		os_p<<std::endl;
	}
	return os_p;
}

}
