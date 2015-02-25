#pragma once

#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>

/**
* Oriented graph with values on each node of given type
* @tparam T type of values on nodes
*/
template<typename T>
class graph_t {
public:
    typedef size_t node_handle;
    typedef size_t edge_handle;

    /**
    * Constructs empty graph
    */
    graph_t() = default;

    /**
    * Copies existing graph
    * @param graph value to copy
    */
    graph_t(graph_t const& graph) = default;

    /**
    * Default destructor
    */
    ~graph_t() = default;

    /**
    * Move constructor
    * @param origin value to move
    */
    graph_t(graph_t && origin)
        : nodes(std::move(origin.nodes))
        , payloads(std::move(origin.payloads))
        , from(std::move(origin.from))
        , to(std::move(origin.to))
    {}

    /**
    * Assigns existing graph
    * @param graph value to assign
    * @return this
    */
    graph_t& operator=(graph_t graph) {
        std::swap(nodes, graph.nodes);
        std::swap(payloads, graph.payloads);
        std::swap(from, graph.from);
        std::swap(to, graph.to);
        return *this;
    }

    /**
    * Checks if this graph is equal to given
    * @param graph value to compare with
    * @return true if this == graph, false othrewise
    */
    bool operator==(graph_t const& graph) const {
        return nodes == graph.nodes
                && payloads == graph.payloads
                && from == graph.from
                && to == graph.to;
    }

    /**
    * Checks if this graph is not equal to given
    * @param graph value to compare with
    * @return true if this != graph, false othrewise
    */
    bool operator!=(graph_t const& graph) const {
        return !(*this == graph);
    }

    /**
    * Loads graph from file with given name to this instance discarding any existing data in this instance.
    * @param filename to load graph data from
    */
    void load_from_file(std::string const& filename) {
        std::ifstream in(filename.c_str());

        size_t n, m;
        in >> n >> m;

        payloads.assign(n, T());

        for (size_t i = 0; i < n; ++i) {
            in >> payloads[i];
        }

        nodes.assign(n, {});
        from.assign(m, 0);
        to.assign(m, 0);

        for (size_t i = 0; i < m; ++i) {
            in >> from[i] >> to[i];
            nodes[from[i]].push_back(i);
        }
    }

    /**
    * Saves graph data to file with given name.
    * @param filename to load graph data from
    */
    void save_to_file(std::string const& filename) const {
        std::ofstream out(filename.c_str());

        out << nodes.size() << " " << from.size() << std::endl;

        for (size_t i = 0; i < payloads.size(); ++i) {
            out << payloads[i] << " ";
        }
        out << std::endl;

        for (size_t i = 0; i < from.size(); ++i) {
            out << from[i] << " " << to[i] << std::endl;
        }
    }

    /**
    * Creates new node and returns it's handle
    * @return handle to the newly created node
    */
    node_handle add_node() {
        nodes.resize(nodes.size() + 1);
        payloads.resize(payloads.size() + 1);
        return nodes.size() - 1;
    }

    /**
    * Adds new edges with given ends and returns it's handle
    * @param a start node handle
    * @param b end node handle
    * @return handle to the newly created edge
    */
    edge_handle add_edge(node_handle const& a, node_handle const& b) {
        from.push_back(a);
        to.push_back(b);
        edge_handle edge = from.size() - 1;
        nodes[a].push_back(edge);
        return edge;
    }

    /**
    * Execute given visitor on each node of this graph.
    * @tparam NodeVisitor type of node visitor
    * @param visitor to execute
    */
    template<typename NodeVisitor>
    void for_each_node(NodeVisitor visitor) const {
        for (node_handle node = 0; node < nodes.size(); ++node)
            visitor(node);
    }

    /**
    * Returns number of nodes in this graph
    * @return numver of nodes in this graph
    */
    size_t get_nodes_count() const {
        return nodes.size();
    }

    /**
    * Executes given visitor on each edge starting at the given node.
    * @param source node
    * @param visitor to execute
    * @tparam EdgeVisitor type of visitor
    */
    template<typename EdgeVisitor>
    void for_each_edge(node_handle const& source, EdgeVisitor visitor) {
        for (edge_handle edge : nodes[source])
            visitor(edge);
    }

    /**
    * Returns end node of given edge if it's start matches given origin
    * @param origin start of edge
    * @param edge to move along
    * @throws std::runtime_error if given edge does not start at the given node
    */
    node_handle move(node_handle const& origin, edge_handle const& edge) {
        if (from[edge] != origin)
            throw std::runtime_error("given edge doesn't start at the given origin");
        return to[edge];
    }

    /**
    * Returns reference to the value on given node
    * @param node
    * @return reference to the value on given node
    */
    T & operator[](node_handle const& node) {
        return payloads[node];
    }

    /**
    * Depth first search. Visits each node and each edge which are reachable from given start node.
    * @param start_node node to start dfs from
    * @param start_visitor to execute when algorithm enters a node
    * @param end_visitor to execute when algorithm leaves a node
    * @param discover_visitor to execute when algorithm discovers a node
    * @tparam StartVisitor type of start_visitor
    * @tparam EndVisitor type of end_visitor
    * @tparam DiscoverVisitor type of discover_visitor
    * @see http://en.wikipedia.org/wiki/Depth-first_search
    */
    template<typename StartVisitor, typename EndVisitor, typename DiscoverVisitor>
    void dfs(node_handle start_node, StartVisitor start_visitor, EndVisitor end_visitor, DiscoverVisitor discover_visitor) {
        if (nodes.empty())
            return;

        std::stack<std::pair<node_handle, size_t>> way;
        std::vector<bool> used(nodes.size());

        way.push(std::make_pair(start_node, 0));
        while (!way.empty()) {
            auto& top = way.top();
            node_handle node = top.first;
            if (!used[node]) {
                used[node] = true;
                start_visitor(node);
            }
            size_t i = top.second++;
            if (i == nodes[node].size()) {
                way.pop();
                end_visitor(node);
                continue;
            }
            edge_handle e = nodes[node][i];
            node_handle next = to[e];
            discover_visitor(next);
            if (used[next])
                continue;
            way.push(std::make_pair(next, 0));
        }
    }

private:
    std::vector<std::vector<edge_handle>> nodes;
    std::vector<T> payloads;

    std::vector<node_handle> from;
    std::vector<node_handle> to;
};