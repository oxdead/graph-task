
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <iostream>
#include <utility>
#include <algorithm>

#define LOFI_DEBUG
#define NIL UINT32_MAX

struct Edge
{
	size_t id;
	size_t a;
	size_t b;
	size_t len;

	Edge(size_t route_id, size_t srcnode, size_t dstnode, size_t edge_len) : id(route_id), a(srcnode), b(dstnode), len(edge_len)
	{
	}

};


struct Node {
	std::vector<size_t> routes;

	Node() 
	{
	}

};


void InitTask(std::vector<Edge>& edges)
{
	edges.emplace_back(Edge(0, 0, 1, 4));
	edges.emplace_back(Edge(1, 1, 2, 8));
	edges.emplace_back(Edge(2, 2, 3, 7));
	edges.emplace_back(Edge(3, 3, 4, 9));
	edges.emplace_back(Edge(4, 4, 5, 10));
	edges.emplace_back(Edge(5, 5, 6, 2));
	edges.emplace_back(Edge(6, 6, 7, 1));
	edges.emplace_back(Edge(7, 7, 0, 8));
	edges.emplace_back(Edge(8, 1, 7, 11));
	edges.emplace_back(Edge(9, 7, 8, 7));
	edges.emplace_back(Edge(10, 2, 8, 2));
	edges.emplace_back(Edge(11, 6, 8, 6));
	edges.emplace_back(Edge(12, 2, 5, 4));
	edges.emplace_back(Edge(13, 3, 5, 14));
}


void PushEdgeToNode(std::map<size_t, Node>& nodes, size_t nid, size_t eid)
{
	if (nid >= NIL) { return; }

	auto it = nodes.find(nid);
	if (it == nodes.end())
	{
		nodes.insert(std::make_pair(nid, Node()));
		it = nodes.find(nid);
	}

	if (it != nodes.end())
	{
		it->second.routes.push_back(eid);
	}

}


// returns vector index
size_t FindRouteById(const std::vector<Edge>& edges, size_t id)
{
	for (size_t i = 0; i < edges.size(); ++i)
	{
		if (id == edges[i].id)
		{
			return i;
		}
	}
	return NIL;
}

bool IsInPassedNodes(std::vector<size_t>& passed_nodes, size_t id)
{
	for (size_t i = 0; i < passed_nodes.size(); ++i)
	{
		if (passed_nodes[i] == id)
		{
			return true;
		}
	}
	return false;
}

size_t NextNode_WebLen(const std::map<size_t, Node>& nodes, size_t nid, std::vector<size_t>& passed_nodes, std::vector<Edge>& edges, std::vector<Edge>& e)
{
	passed_nodes.push_back(nid);
	if (passed_nodes.size() >= nodes.size()) { return 0; }

	
	//loop through all edges for current node, add them to e vector edge collection and remove them from edges
	std::map<size_t, Node>::const_iterator it = nodes.find(nid);
	if (it == nodes.end()) { return 0; }

	for (size_t i = 0; i < it->second.routes.size(); ++i)
	{
		size_t rid = FindRouteById(edges, it->second.routes[i]);
		if (rid < NIL)
		{
			//add only if this edge leads to some unvisited node
			if ((!IsInPassedNodes(passed_nodes, edges[rid].a)) || (!IsInPassedNodes(passed_nodes, edges[rid].b)))
			{
				e.push_back(edges[rid]);
			}

			edges.erase(edges.begin() + rid);
		}
	}


	//find shortest of collection
	size_t len = NIL;
	size_t nextNode = NIL;
	size_t e_cur = NIL;
	for (size_t i = 0; i < e.size(); ++i)
	{
		if (e[i].len < len)
		{
			if (!IsInPassedNodes(passed_nodes, e[i].a))
			{
				nextNode = e[i].a;
				len = e[i].len;
				e_cur = i;
			}
			else if (!IsInPassedNodes(passed_nodes, e[i].b))
			{
				nextNode = e[i].b;
				len = e[i].len;
				e_cur = i;
			}
		}
	}


	if (e_cur < NIL) { e.erase(e.begin() + e_cur); }

	if (len < NIL && nextNode < NIL)
	{
#ifdef LOFI_DEBUG
		std::cout << nextNode << std::endl;
		std::cout << "len:" << len << std::endl;
#endif
		size_t next_len = NextNode_WebLen(nodes, nextNode, passed_nodes, edges, e);
		return len + next_len;
	}

	return 0;


}




bool PushNodePathLength(std::map<size_t, size_t>& shortest_paths, size_t nid, size_t len)
{
	if (nid >= NIL || len >= NIL) { return false; }

	auto it = shortest_paths.find(nid);
	if (it == shortest_paths.end())
	{
		shortest_paths.insert(std::make_pair(nid, len));
		return true;
	}
	else
	{
		if (len < it->second)
		{
			it->second = len;
			return true;
		}
	}

	return false;
}


void NextNode_Shortest(const std::map<size_t, Node>& nodes, size_t prevnid, size_t nid, const std::vector<Edge>& edges, size_t len, std::map<size_t, size_t>& shortest_paths)
{
	if (!PushNodePathLength(shortest_paths, nid, len)) { return; }

	auto it = nodes.find(nid);
	if (it != nodes.end())
	{
		for (size_t i = 0; i < it->second.routes.size(); ++i)
		{
			size_t eid = FindRouteById(edges, it->second.routes[i]);
			if (eid < NIL)
			{
				size_t dst_node = (edges[eid].a != nid) ? edges[eid].a : edges[eid].b;
				if(dst_node != prevnid) // make sure you don't go back same route
				{
					NextNode_Shortest(nodes, nid, dst_node, edges, len + edges[eid].len, shortest_paths);
				}
			}
		}
	}
}




// find shortest length to connect all nodes into a web
size_t ShortestLengthWeb(const std::map<size_t, Node>& nodes, const std::vector<Edge>& edges)
{
	// copy edges to temp vector to freely alter/remove without affecting original 
	std::vector<Edge> edges_copy;
	std::copy(edges.begin(), edges.end(), std::back_inserter(edges_copy));

	std::vector<size_t> passed_nodes; // store passed nodes here
	std::vector<Edge> e; // store already revealed and currently accessible edges here
	size_t maxlen = NextNode_WebLen(nodes, 0, passed_nodes, edges_copy, e); // length of shortest web
	std::cout << maxlen << std::endl; // value or 0 for invalid or if only one node exists
	return maxlen;
}


// find shortest paths to all node from source node
void ShortestPaths(const std::map<size_t, Node>& nodes, const std::vector<Edge>& edges, std::map<size_t, size_t>& shortest_paths)
{
	size_t len = 0;
	NextNode_Shortest(nodes, nodes.size(), 0, edges, len, shortest_paths); // set nodes.size() as prev node for first iteration

#ifdef LOFI_DEBUG
	int counter = 0;
	for (auto it = shortest_paths.begin(); it != shortest_paths.end(); ++it)
	{
		std::cout << counter++ << ": " << it->first << ": " << it->second << std::endl;
	}
#endif
}




int main()
{
	// input conditions
	std::vector<Edge> edges;
	InitTask(edges);

	//generate map of nodes and tie with route identifiers
	std::map<size_t, Node> nodes;
	for (Edge& edge : edges)
	{
		PushEdgeToNode(nodes, edge.a, edge.id);
		PushEdgeToNode(nodes, edge.b, edge.id);
	}

	// algo 1
	size_t maxlen = ShortestLengthWeb(nodes, edges); // length of shortest web

	// algo 2: dijkstra
	std::map<size_t, size_t> shortest_node_paths; // shortest paths to every node
	ShortestPaths(nodes, edges, shortest_node_paths);

    return 0;
}

