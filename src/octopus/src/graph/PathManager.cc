#include "PathManager.hh"

#include <functional>

namespace octopus
{


/// @brief wait for end of computing
void PathManager::joinCompute()
{
	if(_opened)
	{
		_thread.join();
		_opened = false;
	}
}

/// @brief compute in a new thread
void PathManager::startCompute(long long iter_p)
{
	_thread = std::thread([this, iter_p] { compute(iter_p); });
	_opened = true;
}

/// @brief compute a given number of iteration of flow field
/// by poping queries
void PathManager::compute(long long iter_p)
{
	long long cur_l = iter_p;

	while(cur_l > 0 && !_queries.empty())
	{
		FlowFieldQuery &query_l = _queries.front();

		// start if not started
		if(query_l.computation == nullptr)
		{
			// create computation unit
			query_l.computation = new FlowFieldComputation(_internalGrid, query_l.x, query_l.y);
			// init status of query
			query_l.status = _gridStatus;
		}

		cur_l = query_l.computation->compute(cur_l);

		// end it if over
		if(query_l.computation->over())
		{
			// add result
			FlowFieldResult & result_l = _completed[std::make_pair(query_l.x, query_l.y)];
			result_l.status = query_l.status;
			std::swap(result_l.result, query_l.computation->result);

			// remove from queries
			_setQueries.erase(std::make_pair(query_l.x, query_l.y));
			_queries.pop_front();
		}
	}
}

/// @brief query a flow field computation
void PathManager::queryFlowField(long x, long y)
{
	// check if up to date
	FlowFieldResult & result_l = _completed[std::make_pair(x, y)];
	if(result_l.status >= _gridStatus)
	{
		return;
	}

	// add to query
	if(_setQueries.insert(std::make_pair(x,y)).second)
	{
		_queries.push_back(FlowFieldQuery{x, y, 0});
	}
}

/// @brief return a flow field from a target
/// @note return null if no flow field have been computed ever
FlowField const * PathManager::getFlowField(long x, long y) const
{
	auto &&it_l = _completed.find(std::make_pair(x, y));
	if(it_l != _completed.end() && !it_l->second.result.empty())
	{
		return &it_l->second.result;
	}
	return nullptr;
}

/// @brief update internal grid and update status
void PathManager::initFromGrid(std::vector<std::vector<GridNode *> > const &grid_p, unsigned long status_p)
{
	// skip if same status
	if(_gridStatus >= (long long)status_p)
	{
		return;
	}
	if(_internalGrid.empty())
	{
		_internalGrid.reserve(grid_p.size());
		for(size_t i = 0 ; i < grid_p.size() ; ++i)
		{
			_internalGrid.push_back(std::vector<GridNode *>());
			_internalGrid.back().reserve(grid_p[i].size());
			for(size_t j = 0 ; j < grid_p[i].size() ; ++ j)
			{
				_internalGrid[i].emplace_back(new GridNode(grid_p[i][j]->getPosition()));
			}
		}
	}
	for(size_t i = 0 ; i < grid_p.size() ; ++i)
	{
		for(size_t j = 0 ; j < grid_p[i].size() ; ++ j)
		{
			_internalGrid[i][j]->setFree(grid_p[i][j]->isFree());
			_internalGrid[i][j]->setContent(grid_p[i][j]->getContent());
		}
	}
	_gridStatus = status_p;
}

} // namespace octopus
