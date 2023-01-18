#ifndef __GRAPH__
#define __GRAPH__

#include <vector>
#include <list>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

#include "GridNode.hh"

namespace octopus
{

class Entity;

struct VertexProperties {
};

struct EdgeProperties {
	GridNode const * from { nullptr };
	GridNode const * to { nullptr };
	double weight { 0. };
};

/// @brief This class represent a graph based on a grid of nodes
/// only adjacent (index wise) nodes will be connected
class Graph
{
public:
	/// @brief create the graph with the given node
	Graph(std::vector<std::vector<GridNode *> > const &nodes_p);
	~Graph();

	std::list<GridNode const *> getPath(GridNode const * from_p, GridNode const * to_p, std::list<Entity const *> const& ignored_p= {}) const;
private:
	// specify some types
	typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties > mygraph_t;

	typedef typename boost::graph_traits< mygraph_t >::vertex_descriptor Vertex;
	typedef typename boost::graph_traits< mygraph_t >::edge_descriptor Edge;

	/// @brief vector of vector of all nodes
	std::vector<std::vector<GridNode *> > const &_nodes;
	/// @brief map on constant access to get vertex id from GridNode
	std::unordered_map<GridNode const *, Vertex> _nodeIndex;
	std::vector<GridNode const*> _vecNodes;
	/// @brief internal graph
	mygraph_t *_g;

	void buildEdge(mygraph_t &g, size_t i, size_t j, size_t k, size_t l,
		std::vector<std::vector<GridNode *> > const &nodes_p, std::unordered_map<GridNode const *, Vertex> const & nodeIndex_p);
};

/// @brief trim path to remove all intermediat node that do not define any change of direction
/// eg : (0,0) (1,0) (2,0) (3,1) (4,2) (5,3) would become (0,0) (2,0) (5,3)
void trimPath(std::list<GridNode const *> &path_p);

/// @brief transorm a path of GridNode to way points for further command
std::list<Vector> toWaypoints(std::list<GridNode const *> &path_p);

} // namespace octopus

#endif
