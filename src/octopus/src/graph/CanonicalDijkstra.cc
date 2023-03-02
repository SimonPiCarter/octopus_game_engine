#include "CanonicalDijkstra.hh"

#include <map>
#include <list>
#include <set>
#include <cassert>

#include "utils/Box.hh"

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

Box<long> getBoxFromContent(std::vector<std::vector<GridNode *> > const &grid_p, long x, long y)
{
	Entity const * content_l = grid_p[x][y]->getContent();
	Box<long> box_l;

	for(long mx = x ; mx >= 0 ; --mx)
	{
		if(grid_p[mx][y]->getContent() != content_l)
		{
			box_l._lowerX = mx;
			break;
		}
	}
	for(long mx = x ; mx < grid_p.size() ; ++mx)
	{
		if(grid_p[mx][y]->getContent() != content_l)
		{
			box_l._upperX = mx;
			break;
		}
	}

	for(long my = y ; my >= 0 ; --my)
	{
		if(grid_p[x][my]->getContent() != content_l)
		{
			box_l._lowerY = my;
			break;
		}
	}
	for(long my = y ; my < grid_p[x].size() ; ++my)
	{
		if(grid_p[x][my]->getContent() != content_l)
		{
			box_l._upperY = my;
			break;
		}
	}

	return box_l;
}

std::set<ValueNode, ValueSorter> buildOpenNodes(std::vector<std::vector<GridNode *> > const &grid_p, long x, long y)
{
	if(grid_p[x][y]->isFree())
	{
		return {ValueNode{x, y, 0.}};
	}
	Box<long> box_l = getBoxFromContent(grid_p, x, y);

	std::set<ValueNode, ValueSorter> open_l;

	// increase search
	for(long extension_l = 0; extension_l < 100 ; ++extension_l)
	{
		for(long i = box_l._lowerX-extension_l; i <= box_l._upperX+extension_l ; ++ i)
		{
			if(i >= 0 && i < grid_p.size())
			{
				for(long j = box_l._lowerY-extension_l; j <= box_l._upperY+extension_l ; ++ j)
				{
					if(j >= 0 && j < grid_p[i].size()
					&& grid_p[i][j]->isFree())
					{
						open_l.insert(ValueNode{i, j, 0.});
					}
				}
			}
		}

		// found empty > return
		if(!open_l.empty())
		{
			return open_l;
		}
	}

	return open_l;
}

ValueGrid canonical_dijkstra(std::vector<std::vector<GridNode *> > const &grid_p, long x, long y)
{
	ValueGrid grid_l;
	/// init all node in closed with -1 (to mean +infinity)
	for(long i = 0 ; i < grid_p.size() ; ++i)
	{
		grid_l.emplace_back(grid_p[i].size(), Fixed(-1.));
	}

	std::set<ValueNode, ValueSorter> open_l(buildOpenNodes(grid_p, x, y));

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
	Fixed best_l(grid_p[x][y]);
	for(long i : {0, -1, 1})
	{
		// skip out of bound
		if(x+i < 0 || x+i >= grid_p.size())
		{
			continue;
		}
		for(long j : {0, -1, 1})
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

Vector obstacle_field(ValueGrid const &grid_p, long x, long y)
{
	Fixed dx = 0;
	Fixed dy = 0;
	for(long i : {0, -1, 1})
	{
		bool oob_l = false;
		// skip out of bound
		if(x+i < 0 || x+i >= grid_p.size())
		{
			oob_l = true;
		}
		for(long j : {0, -1, 1})
		{
			// skip out of bound
			if(!oob_l
			&&(y+j < 0 || y+j >= grid_p[x+i].size()))
			{
				oob_l = true;
			}
			if(!oob_l && grid_p[x+i][y+j] >= 0. )
			{
				dx += i;
				dy += j;
			}
		}
	}
	if(dx < 1e-3)
	{
		dx = -1.;
	}
	else if(dx > 1e-3)
	{
		dx = 1.;
	}
	if(dy < 1e-3)
	{
		dy = -1.;
	}
	else if(dy > 1e-3)
	{
		dy = 1.;
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
			if(grid_p[i][j] >= 0.)
			{
				result_l[i][j] = compute_field(grid_p, i, j);
			}
			else
			{
				result_l[i][j] = obstacle_field(grid_p, i, j);
			}
		}
	}
	return result_l;
}

Vector direction(Fixed x, Fixed y, FlowField const &field_p)
{
	long long floorX_l = (x).to_int();
	long long floorY_l = (y).to_int();

	Fixed ratioX_l = x - floorX_l;
	Fixed ratioY_l = y - floorY_l;

	// direction is the sum of all vectors ponderated by the position in the floored node

	// coef based on dx and dy
	std::map<long, Fixed> coefX_l;
	coefX_l[-1] = 1. - ratioX_l;
	coefX_l[0] = 1.;
	coefX_l[1] = ratioX_l;
	std::map<long, Fixed> coefY_l;
	coefY_l[-1] = 1. - ratioY_l;
	coefY_l[0] = 1.;
	coefY_l[1] = ratioY_l;

	Vector result_l;
	for(long i = -1 ; i < 2 ; ++i)
	{
		// skip out of bound
		if(x+i < 0 || x+i >= field_p.size())
		{
			continue;
		}
		for(long j = -1 ; j < 2 ; ++j)
		{
			// skip out of bound
			if(y+j < 0 || y+j >= field_p[floorX_l+i].size())
			{
				continue;
			}

			result_l += field_p[floorX_l+i][floorY_l+j] * coefX_l[i] * coefY_l[j];
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

FlowFieldComputation::FlowFieldComputation(std::vector<std::vector<GridNode *> > const &grid_p, long x_p, long y_p)
	: x(x_p)
	, y(y_p)
	, grid(grid_p)
{
	/// init all node in closed with -1 (to mean +infinity)
	for(long i = 0 ; i < grid_p.size() ; ++i)
	{
		valueGrid.emplace_back(grid_p[i].size(), Fixed(-1.));
	}

	open = buildOpenNodes(grid_p, x, y);
}

/// @brief compute some iterations for flow field
/// @return the number of remaining iteration if finished
long long FlowFieldComputation::compute(long long iter_p)
{
	long long iterRemaining_l = iter_p;

	// finish entries if any
	while(!entries.empty() && iterRemaining_l > 0)
	{
		canonical_entries entry_l = entries.front();
		entries.pop_front();

		--iterRemaining_l;
		canonical_ordering(entry_l.child, entry_l.parent, entry_l.cost, valueGrid, open, grid, entries);
	}

	while(!open.empty() && iterRemaining_l > 0)
	{
		ValueNode best_l = *open.begin();
		open.erase(open.begin());

		--iterRemaining_l;
		canonical_ordering(best_l, best_l, best_l.cost, valueGrid, open, grid, entries);

		while(!entries.empty() && iterRemaining_l > 0)
		{
			canonical_entries entry_l = entries.front();
			entries.pop_front();

			--iterRemaining_l;
			canonical_ordering(entry_l.child, entry_l.parent, entry_l.cost, valueGrid, open, grid, entries);
		}
	}

	// if over we compute the flow field
	if(over())
	{
		result = flow_field(valueGrid);
	}

	// return the number of iteration remaining for next query
	return iterRemaining_l;
}

/// @return true if the flow field has been computed
bool FlowFieldComputation::over() const
{
	return entries.empty() && open.empty();
}

}
