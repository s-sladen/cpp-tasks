#include <vector>
#include <fstream>
#include <stack>

template<typename T>
class graph {
public:
    typedef size_t node_handle;
    typedef size_t edge_handle;

    void load_from_file(std::string const & filename) {
        std::ifstream in(filename.c_str());

        size_t n, m;
        in >> n >> m;

        payloads.assign(n);

        for (size_t i = 0; i < n; ++i) {
            in >> payloads[i];
        }

        nodes.assign(n);
        from.assign(m);
        to.assign(m);

        for (size_t i = 0; i < m; ++i) {
            in >> from[i] >> to[i];
            nodes[from[i]].push_back(i);
        }
    }

    void save_to_file(std::string const & filename) const {
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

    void add_edge(node_handle const & a, node_handle const & b) {
        from.push_back(a);
        to.push_back(b);
        nodes[a].push_back(from.size() - 1);
    }

    template<typename NodeVisitor>
    void for_each_node(NodeVisitor visitor) const {
        for (node_handle node = 0; node < nodes.size(); ++node)
            visitor(node);
    }

    size_t get_nodes_count() const {
        return nodes.size();
    }

    template<typename EdgeVsitor>
    void for_each_edge(node_handle const & source, EdgeVsitor visitor) {
        for (edge_handle edge : nodes[source])
            visitor(edge);
    }

    node_handle move(node_handle const & origin, edge_handle const & edge) {
        if (from[edge] != origin)
            throw std::runtime_error("given edge doesn't start at the given origin");
        return to[edge];
    }

    T & operator[](node_handle const & node) {
        return payloads[node];
    }

    template<typename StartVisitor, typename EndVisitor, typename DiscoverVisitoe>
    void dfs(StartVisitor start_node, EndVisitor end_node, DiscoverVisitoe discover_node) {
        if (nodes.empty())
            return;

        std::stack<node_handle> way;
        std::stack<size_t> used;
        std::vector<bool> was(nodes.size());

        node_handle last = 0;

        start_node(0);
        way.push(0);
        used.push(0);
        was[0] = true;

        while (!way.empty()) {
            node_handle node = way.top();
            size_t u = used.top()++;
            if (u == nodes[node].size()) {
                way.pop();
                used.pop();
                continue;
            }
            edge_handle e = nodes[node][u];
            node_handle next = to[e];
            if (was[next])
                continue;
            discover_node(next);
            way.push(next);
            used.push(0);
            was[next] = true;
            last = next;
        }

        end_node(last);
    }

private:
    std::vector<std::vector<edge_handle>> nodes;
    std::vector<T> payloads;

    std::vector<node_handle> from;
    std::vector<node_handle> to;
};

int main() {

}