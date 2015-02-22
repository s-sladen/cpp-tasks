#pragma once

#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>

template<typename T>
class graph_t {
public:
    typedef size_t node_handle;
    typedef size_t edge_handle;

    graph_t() = default;
    graph_t(graph_t const&) = default;
    ~graph_t() = default;

    graph_t(graph_t && origin)
        : nodes(std::move(origin.nodes))
        , payloads(std::move(origin.payloads))
        , from(std::move(origin.from))
        , to(std::move(origin.to))
    {}

    graph_t& operator=(graph_t graph) {
        std::swap(nodes, graph.nodes);
        std::swap(payloads, graph.payloads);
        std::swap(from, graph.from);
        std::swap(to, graph.to);
        return *this;
    }

    bool operator==(graph_t const& graph) const {
        return nodes == graph.nodes
                && payloads == graph.payloads
                && from == graph.from
                && to == graph.to;
    }

    bool operator!=(graph_t const& graph) const {
        return !(*this == graph);
    }

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

    node_handle add_node() {
        nodes.resize(nodes.size() + 1);
        payloads.resize(payloads.size() + 1);
        return nodes.size() - 1;
    }

    edge_handle add_edge(node_handle const& a, node_handle const& b) {
        from.push_back(a);
        to.push_back(b);
        edge_handle edge = from.size() - 1;
        nodes[a].push_back(edge);
        return edge;
    }

    template<typename NodeVisitor>
    void for_each_node(NodeVisitor visitor) const {
        for (node_handle node = 0; node < nodes.size(); ++node)
            visitor(node);
    }

    size_t get_nodes_count() const {
        return nodes.size();
    }

    template<typename EdgeVisitor>
    void for_each_edge(node_handle const& source, EdgeVisitor visitor) {
        for (edge_handle edge : nodes[source])
            visitor(edge);
    }

    node_handle move(node_handle const& origin, edge_handle const& edge) {
        if (from[edge] != origin)
            throw std::runtime_error("given edge doesn't start at the given origin");
        return to[edge];
    }

    T & operator[](node_handle const& node) {
        return payloads[node];
    }

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