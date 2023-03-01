#ifndef __PathManager__
#define __PathManager__

#include <list>
#include <set>
#include <vector>
#include <map>

#include "graph/CanonicalDijkstra.hh"

namespace octopus
{

struct FlowFieldQuery
{
	~FlowFieldQuery() { delete computation; }
	long x {0}, y {0};
	unsigned long status {0};

	FlowFieldComputation * computation {nullptr};
};

struct FlowFieldResult
{
	FlowField result;
	unsigned long status {0};
};

class PathManager
{
public:
	/// @brief compute a given number of iteration of flow field
	void compute(long long iter_p);

	/// @brief query a flow field computation
	void queryFlowField(long x, long y);

	/// @brief return a flow field from a target
	/// @note return null if no flow field have been computed ever
	FlowField const * getFlowField(long x, long y) const;

	/// @brief update internal grid and update status
	void initFromGrid(std::vector<std::vector<GridNode *> > const &grid_p, unsigned long status_p);

	/// @brief getter for grid status
	unsigned long getGridStatus() const { return _gridStatus; }

	std::vector<std::vector<GridNode *> > const & getInternalGrid() const { return _internalGrid; }
protected:
	/// @brief all grid node storred by indexes
	std::vector<std::vector<GridNode *> > _internalGrid;

	/// @brief used to know if grid has changed
	unsigned long _gridStatus {0};

	/// @brief list of queued queries
	std::list<FlowFieldQuery> _queries;
	std::set<std::pair<long, long> > _setQueries;

	/// @brief map of computed queries
	std::map<std::pair<long, long>, FlowFieldResult> _completed;
};
} // namespace octopus


#endif
