#ifndef __PathManager__
#define __PathManager__

#include <list>
#include <set>
#include <vector>
#include <map>
#include <thread>

#include "graph/CanonicalDijkstra.hh"

namespace octopus
{

struct FlowFieldQuery
{
	~FlowFieldQuery() { delete computation; }
	long x {0}, y {0};
	long long status {0};

	FlowFieldComputation * computation {nullptr};
};

struct FlowFieldResult
{
	FlowField result;
	// Less than -1 to trigger query even when non intialized
	long long status {-10};
};

class PathManager
{
public:
	/// @brief wait for end of computing
	void joinCompute();

	/// @brief compute in a new thread
	void startCompute(long long iter_p);

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
	/// @brief thread for parallel computing
	std::thread _thread;
	bool _opened {false};

	/// @brief all grid node storred by indexes
	std::vector<std::vector<GridNode *> > _internalGrid;

	/// @brief used to know if grid has changed
	long long _gridStatus {-1};

	/// @brief list of queued queries
	std::list<FlowFieldQuery> _queries;
	std::set<std::pair<long, long> > _setQueries;

	/// @brief map of computed queries
	std::map<std::pair<long, long>, FlowFieldResult> _completed;
};
} // namespace octopus


#endif
