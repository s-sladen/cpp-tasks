#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Graph
#include <boost/test/unit_test.hpp>

#include <graph.h>
#include <set>
#include <queue>

BOOST_AUTO_TEST_CASE(test_nodes_manipulation)
{
    graph_t<std::string> g;
    typedef decltype(g)::node_handle node_handle;
    auto a = g.add_node();
    g[a] = "hello";
    auto b = g.add_node();
    g[b] = "world";
    auto c = g.add_node();
    g[c] = "!";
    BOOST_CHECK_NE(a, b);
    BOOST_CHECK_NE(b, c);
    BOOST_CHECK_NE(a, c);
    BOOST_CHECK_EQUAL(g[a], "hello");
    BOOST_CHECK_EQUAL(g[b], "world");
    BOOST_CHECK_EQUAL(g[c], "!");
    BOOST_CHECK_EQUAL(g.get_nodes_count(), 3);

    std::set<node_handle> visited;
    g.for_each_node([&visited, a, b, c] (node_handle const& handle) {
        BOOST_CHECK(visited.insert(handle).second);
        BOOST_CHECK(handle == a || handle == b || handle == c);
    });
    BOOST_CHECK(visited.count(a) && visited.count(b) && visited.count(c));
    BOOST_CHECK_EQUAL(visited.size(), 3);
}

BOOST_AUTO_TEST_CASE(test_edges_manipulation)
{
    graph_t<int> g;
    typedef decltype(g)::edge_handle edge_handle;

    auto a = g.add_node();
    auto b = g.add_node();
    auto c = g.add_node();

    auto d = g.add_edge(a, b);
    auto e = g.add_edge(c, b);
    auto f = g.add_edge(a, c);

    BOOST_CHECK_EQUAL(g.move(a, d), b);
    BOOST_CHECK_EQUAL(g.move(c, e), b);
    BOOST_CHECK_EQUAL(g.move(a, f), c);

    BOOST_CHECK_THROW(g.move(a, e), std::runtime_error);

    BOOST_CHECK_THROW(g.move(b, d), std::runtime_error);
    BOOST_CHECK_THROW(g.move(b, e), std::runtime_error);
    BOOST_CHECK_THROW(g.move(b, f), std::runtime_error);

    BOOST_CHECK_THROW(g.move(c, d), std::runtime_error);
    BOOST_CHECK_THROW(g.move(c, f), std::runtime_error);

    std::set<edge_handle> visited;
    g.for_each_edge(a, [&visited, d, f](edge_handle const& handle) {
        BOOST_CHECK(visited.insert(handle).second);
        BOOST_CHECK(handle == d || handle == f);
    });
    BOOST_CHECK(visited.count(d) && visited.count(f));
    BOOST_CHECK_EQUAL(visited.size(), 2);

    visited.clear();
    g.for_each_edge(b, [](edge_handle const& handle) {
        BOOST_FAIL("must not enter here");
    });
    BOOST_CHECK(visited.empty());

    g.for_each_edge(c, [&visited, e](edge_handle const& handle) {
        BOOST_CHECK(visited.insert(handle).second);
        BOOST_CHECK_EQUAL(handle, e);
    });
    BOOST_CHECK(visited.count(e));
    BOOST_CHECK_EQUAL(visited.size(), 1);
}

BOOST_AUTO_TEST_CASE(test_dfs)
{
    graph_t<int> g;
    typedef decltype(g)::node_handle node_handle;

    auto a = g.add_node();
    auto b = g.add_node();
    auto c = g.add_node();
    auto d = g.add_node();
    auto e = g.add_node();
    auto f = g.add_node();

    /*
     f
     ^
     |
     d-->e
     ^   ^
     |   |
     b-->c
     ^   ^
      \ /
       a
     */

    g.add_edge(a, b);
    g.add_edge(b, c);
    g.add_edge(a, c);
    g.add_edge(b, d);
    g.add_edge(c, e);
    g.add_edge(d, e);
    g.add_edge(d, f);

    enum event_type {
        START_NODE,
        END_NODE,
        DISCOVER_NODE
    };

    std::queue<std::pair<event_type, node_handle>> expect;
    expect.push(std::make_pair(START_NODE, a));
    expect.push(std::make_pair(DISCOVER_NODE, b));
    expect.push(std::make_pair(START_NODE, b));
    expect.push(std::make_pair(DISCOVER_NODE, c));
    expect.push(std::make_pair(START_NODE, c));
    expect.push(std::make_pair(DISCOVER_NODE, e));
    expect.push(std::make_pair(START_NODE, e));
    expect.push(std::make_pair(END_NODE, e));
    expect.push(std::make_pair(END_NODE, c));
    expect.push(std::make_pair(DISCOVER_NODE, d));
    expect.push(std::make_pair(START_NODE, d));
    expect.push(std::make_pair(DISCOVER_NODE, e));
    expect.push(std::make_pair(DISCOVER_NODE, f));
    expect.push(std::make_pair(START_NODE, f));
    expect.push(std::make_pair(END_NODE, f));
    expect.push(std::make_pair(END_NODE, d));
    expect.push(std::make_pair(END_NODE, b));
    expect.push(std::make_pair(DISCOVER_NODE, c));
    expect.push(std::make_pair(END_NODE, a));

    g.dfs(a, [&expect] (node_handle const& handle) {
        BOOST_CHECK_EQUAL(expect.front().first, START_NODE);
        BOOST_CHECK_EQUAL(expect.front().second, handle);
        expect.pop();
    }, [&expect] (node_handle const& handle) {
        BOOST_CHECK_EQUAL(expect.front().first, END_NODE);
        BOOST_CHECK_EQUAL(expect.front().second, handle);
        expect.pop();
    }, [&expect] (node_handle const& handle) {
        BOOST_CHECK_EQUAL(expect.front().first, DISCOVER_NODE);
        BOOST_CHECK_EQUAL(expect.front().second, handle);
        expect.pop();
    });
    BOOST_CHECK(expect.empty());
}

BOOST_AUTO_TEST_CASE(check_file_operations)
{
    std::string filename = "graph.txt";
    graph_t<int> g;

    auto a = g.add_node();
    auto b = g.add_node();
    auto c = g.add_node();
    auto d = g.add_node();
    auto e = g.add_node();
    auto f = g.add_node();

    g[a] = 1;
    g[b] = 300;
    g[c] = -20;
    g[d] = 500;
    g[e] = -100;
    g[f] = 20;


    /*
     f
     ^
     |
     d-->e
     ^   ^
     |   |
     b-->c
     ^   ^
      \ /
       a
     */

    g.add_edge(a, b);
    g.add_edge(b, c);
    g.add_edge(a, c);
    g.add_edge(b, d);
    g.add_edge(c, e);
    g.add_edge(d, e);
    g.add_edge(d, f);

    g.save_to_file(filename);

    graph_t<int> h;
    h.load_from_file(filename);
    BOOST_CHECK(g == h);
}