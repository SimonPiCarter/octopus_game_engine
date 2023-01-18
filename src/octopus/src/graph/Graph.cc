#include "Graph.hh"

#include <stdexcept>

#include <boost/graph/astar_search.hpp>
#include <boost/property_map/function_property_map.hpp>

#include "logger/Logger.hh"

namespace octopus
{

/// @brief check that all row have the same size
bool checkNodes(std::vector<std::vector<GridNode *> > const &nodes_p)
{
	size_t size_l = 0;
	bool first_l = true;
	for(std::vector<GridNode *> const &row_l : nodes_p)
	{
		if(first_l)
		{
			size_l = row_l.size();
		}
		first_l = false;
		if(size_l != row_l.size())
		{
			return false;
		}
	}
	return true;
}

void Graph::buildEdge(mygraph_t &g, size_t i, size_t j, size_t k, size_t l,
	std::vector<std::vector<GridNode *> > const &nodes_p, std::unordered_map<GridNode const *, Vertex> const & nodeIndex_p)
{
	typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> mygraph_t;
	typedef mygraph_t::edge_descriptor edge_descriptor;

	GridNode const * from_l = nodes_p.at(i).at(j);
	GridNode const * to_l = nodes_p.at(k).at(l);

	edge_descriptor e;
	bool inserted;

	boost::tie(e, inserted) = add_edge(nodeIndex_p.at(from_l), nodeIndex_p.at(to_l), g);
	g[e].weight = length(to_l->getPosition() - from_l->getPosition());
	g[e].from = from_l;
	g[e].to = to_l;
}

double weight_adjusted(
	boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> const &g,
	boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties>::edge_descriptor e)
{
	EdgeProperties const &prop_l = g[e];
	double w_l = prop_l.weight;
	if(!prop_l.from->isFree())
	{
		w_l += 5000.;
	}
	if(!prop_l.to->isFree())
	{
		w_l += 5000.;
	}
	return w_l;
}

Graph::Graph(std::vector<std::vector<GridNode *> > const &nodes_p)
	: _nodes(nodes_p)
	, _g(new mygraph_t())
{
	if(!checkNodes(_nodes))
	{
		throw std::logic_error("Cannot create graph with different number of node per row!");
	}
	_g = new mygraph_t();

	for(size_t i = 0 ; i < _nodes.size() ; ++i)
	{
		for(size_t j = 0 ; j < _nodes.at(i).size() ; ++j)
		{
			_nodeIndex[_nodes.at(i).at(j)] = add_vertex(*_g);
			_vecNodes.push_back(_nodes.at(i).at(j));
		}
	}

	for(size_t i = 0 ; i < _nodes.size() ; ++i)
	{
		// skip last row
		if(i+1 == _nodes.size())
		{
			continue;
		}

		for(size_t j = 0 ; j < _nodes.at(i).size() ; ++j)
		{
			// skip last column
			if(j+1 == _nodes.size())
			{
				continue;
			}

			buildEdge(*_g, i, j, i+1, j, _nodes, _nodeIndex);
			buildEdge(*_g, i, j, i, j+1, _nodes, _nodeIndex);
			buildEdge(*_g, i, j, i+1, j+1, _nodes, _nodeIndex);
		}
	}

	_filtered = new myfilteredgraph_t(*_g, Filter {this});
}

Graph::~Graph()
{
	delete _filtered;
	delete _g;
}

// euclidean distance heuristic
template < class Graph>
class distance_heuristic : public boost::astar_heuristic< Graph, double >
{
public:
	typedef typename boost::graph_traits< Graph >::vertex_descriptor Vertex;

	distance_heuristic(std::vector<GridNode const*> const &vec_p, Vertex goal_p) : _vec(vec_p), _goal(goal_p) {}
	double operator()(Vertex u)
	{
		float dx = _vec.at(_goal)->getPosition().x - _vec.at(u)->getPosition().x;
		float dy = _vec.at(_goal)->getPosition().y - _vec.at(u)->getPosition().y;
		return ::sqrt(dx * dx + dy * dy);
	}

private:
	std::vector<GridNode const*> const & _vec;
	Vertex _goal;
};

struct found_goal
{
}; // exception for termination

// visitor that terminates when we find the goal
template < class Vertex >
class astar_goal_visitor : public boost::default_astar_visitor
{
public:
	astar_goal_visitor(Vertex goal_p) : _goal(goal_p) {}
	template < class Graph > void examine_vertex(Vertex u, Graph&)
	{
		if (u == _goal)
			throw found_goal();
	}

private:
	Vertex _goal;
};
/// cf this for using dynamic weight
/// https://stackoverflow.com/questions/45966930/augment-custom-weights-to-edge-descriptors-in-boostgrid-graph
std::list<GridNode const *> Graph::getPath(GridNode const * from_p, GridNode const * to_p) const
{
	Vertex start_l = _nodeIndex.at(from_p);
	Vertex goal_l = _nodeIndex.at(to_p);

	mygraph_t *graph_l = _g;
	std::vector<Vertex> p(boost::num_vertices(*graph_l));
	std::vector<float> d(boost::num_vertices(*graph_l));
	try
	{
		// custom weight map
		auto custom = boost::make_function_property_map<mygraph_t::edge_descriptor>(
				[&graph_l](mygraph_t::edge_descriptor e) {
					return weight_adjusted(*graph_l, e);
				});

		// // call astar named parameter interface
		boost::astar_search(
			*graph_l,
			start_l,
			distance_heuristic<myfilteredgraph_t>(_vecNodes, goal_l),
			boost::predecessor_map(&p[0])
				.weight_map(custom)
				.distance_map(&d[0])
				.visitor(astar_goal_visitor< Vertex >(goal_l)));
	}
	catch (found_goal fg)
	{ // found a path to the goal
		std::list< GridNode const * > shortest_path_l;
		for (Vertex v = goal_l;; v = p[v])
		{
			shortest_path_l.push_front(_vecNodes[v]);
			if (p[v] == v)
				break;
		}
		Logger::getDebug() << "Shortest path from " << _vecNodes[start_l]->getPosition() << " to " << _vecNodes[goal_l]->getPosition()
			 << ": ";
		std::list< GridNode const * >::iterator spi = shortest_path_l.begin();
		Logger::getDebug() << _vecNodes[start_l];
		for (++spi; spi != shortest_path_l.end(); ++spi)
			Logger::getDebug() << " -> " << (*spi)->getPosition();
		Logger::getDebug() << std::endl << "Total travel time: " << d[goal_l] << std::endl;
		return shortest_path_l;
	}

	Logger::getDebug() << "Didn't find a path from " << _vecNodes[start_l]->getPosition() << "to" << _vecNodes[goal_l]->getPosition()
		 << "!" << std::endl;
	return {};
}

bool Graph::Filter::operator()(boost::graph_traits< mygraph_t >::edge_descriptor const &e) const
{
	Vertex s = boost::source(e, *_graph->_g);
	Vertex t = boost::target(e, *_graph->_g);
	return _graph->_vecNodes[s]->isFree() && _graph->_vecNodes[t]->isFree();
}

void trimPath(std::list<GridNode const *> &path_p)
{
	for(auto it_l = path_p.begin() ; it_l != path_p.end() ;)
	{
		// keep first
		if(it_l == path_p.begin())
		{
			++it_l;
			continue;
		}
		// keep last
		if(std::next(it_l) == path_p.end())
		{
			++it_l;
			continue;
		}

		// compare direction between last and current and current and next
		auto itPast_l = std::prev(it_l);
		auto itNext_l = std::next(it_l);

		Vector lastDir_l = (*it_l)->getPosition() - (*itPast_l)->getPosition();
		Vector nextDir_l = (*itNext_l)->getPosition() - (*it_l)->getPosition();

		if(same_direction(lastDir_l, nextDir_l))
		{
			it_l = path_p.erase(it_l);
		}
		else
		{
			++it_l;
		}
	}
}

std::list<Vector> toWaypoints(std::list<GridNode const *> &path_p)
{
	std::list<Vector> waypoints_l;
	for(GridNode const *node_l : path_p)
	{
		waypoints_l.push_back(node_l->getPosition());
	}
	return waypoints_l;
}

} // namespace octopus

