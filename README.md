# graph
A c++ header-only graph class  
For build, just run ***make***  
### Tutorial  
A very simple c++ example is presented.  
```cpp
#include <iostream>
#include "graph/graph.h"
struct EdgeVisitor {
    void operator()(Graph<int, int>::const_node_iterator left,
                    Graph<int, int>::const_node_iterator right,
                    Graph<int, int>::const_edge_iterator edge) {
        std::cout << "Edge has value " << *edge << ", and connect " << *left
                  << " and " << *right << std::endl;
    }
};

struct NodeVisitor {
    void operator()(Graph<int, int>::const_node_iterator node,
                    Graph<int, int>::const_edge_iterator in_begin,
                    Graph<int, int>::const_edge_iterator in_end,
                    const size_t& in_size,
                    Graph<int, int>::const_edge_iterator out_begin,
                    Graph<int, int>::const_edge_iterator out_end,
                    const size_t& out_size) {
        std::cout << "Node is " << *node << " and has " << in_size
                  << " in edges. " << out_size << " out edges. " << std::endl;
    }
};
struct MutateNodeVisitor {
    void operator()(Graph<int, int>::node_iterator node,
                    Graph<int, int>::edge_iterator in_begin,
                    Graph<int, int>::edge_iterator in_end,
                    const size_t& in_size,
                    Graph<int, int>::edge_iterator out_begin,
                    Graph<int, int>::edge_iterator out_end,
                    const size_t& out_size) {
        *node += 10;
    }
};

int main() {
    // create a graph whose nodes and edges are strings
    Graph<int, int> basic_graph;

    // add some nodes
    Graph<int, int>::node_iterator node1 = basic_graph.add_node(1);
    Graph<int, int>::node_iterator node2 = basic_graph.add_node(2);
    Graph<int, int>::node_iterator node3 = basic_graph.add_node(3);
    Graph<int, int>::node_iterator node4 = basic_graph.add_node(4);

    Graph<int, int>::node_iterator node5 = basic_graph.add_node(5);
    Graph<int, int>::node_iterator node6 = basic_graph.add_node(6);
    // add two edges connecting the nodes
    basic_graph.add_edge(1, node1, node2);
    basic_graph.add_edge(2, node1, node3);
    basic_graph.add_edge(3, node3, node4);
    basic_graph.add_edge(node5, node6);

    // visit all the nodes
    for (Graph<int, int>::node_iterator i = basic_graph.nodes.begin();
         i != basic_graph.nodes.end(); ++i) {
        std::cout << "Node is " << *i << " and has " << i.in_size()
                  << " in edges. " << i.out_size() << " out edges. "
                  << std::endl;
    }

    // visit all the edges
    for (Graph<int, int>::edge_iterator j = basic_graph.edges.begin();
         j != basic_graph.edges.end(); ++j) {
        std::cout << "Edge connects nodes " << *(j.left()) << " and "
                  << *(j.right()) << std::endl;
    }
    std::cout << "================visit edges===============\n";
    EdgeVisitor edge_visitor;
    std::vector<Graph<int, int>::node_iterator> heads{node1, node5};
    visit_edges_depth_first(heads, edge_visitor);
    NodeVisitor node_visitor;
    std::cout << "================visit nodes===============\n";
    visit_nodes_depth_first(heads, node_visitor);
    std::vector<Graph<int, int>::node_iterator> tails{node3, node6};
    std::cout << "==========visit nodes post order==========\n";
    visit_nodes_depth_first_post_order(tails, node_visitor);
    std::cout << "===============mutate nodes ==============\n";
    MutateNodeVisitor mutate_node_visitor;
    visit_nodes_depth_first(heads, mutate_node_visitor);
    std::cout << "=============after mutation ==============\n";
    visit_nodes_depth_first(heads, node_visitor);
    return 0;
}
```  
This program may write follwing message to terminal  
```
Node is 6 and has 1 in edges. 0 out edges.
Node is 5 and has 0 in edges. 1 out edges.
Node is 1 and has 0 in edges. 2 out edges.
Node is 2 and has 1 in edges. 0 out edges.
Node is 3 and has 1 in edges. 1 out edges.
Node is 4 and has 1 in edges. 0 out edges.
Edge connects nodes 1 and 2
Edge connects nodes 1 and 3
Edge connects nodes 3 and 4
Edge connects nodes 5 and 6
================visit edges===============
Edge has value2, and connect 1 and 3
Edge has value3, and connect 3 and 4
Edge has value1, and connect 1 and 2
Edge has value0, and connect 5 and 6
================visit nodes===============
Node is 1 and has 0 in edges. 2 out edges.
Node is 3 and has 1 in edges. 1 out edges.
Node is 4 and has 1 in edges. 0 out edges.
Node is 2 and has 1 in edges. 0 out edges.
Node is 5 and has 0 in edges. 1 out edges.
Node is 6 and has 1 in edges. 0 out edges.
==========visit nodes post order==========
Node is 3 and has 1 in edges. 1 out edges.
Node is 4 and has 1 in edges. 0 out edges.
Node is 6 and has 1 in edges. 0 out edges.
Node is 6 and has 1 in edges. 0 out edges. 
Node is 5 and has 0 in edges. 1 out edges. 
Node is 1 and has 0 in edges. 2 out edges. 
Node is 2 and has 1 in edges. 0 out edges. 
Node is 3 and has 1 in edges. 1 out edges. 
Node is 4 and has 1 in edges. 0 out edges. 
Edge connects nodes 1 and 2
Edge connects nodes 1 and 3
Edge connects nodes 3 and 4
Edge connects nodes 5 and 6
================visit edges===============
Edge has value2, and connect 1 and 3
Edge has value3, and connect 3 and 4
Edge has value1, and connect 1 and 2
Edge has value0, and connect 5 and 6
================visit nodes===============
Node is 1 and has 0 in edges. 2 out edges.
Node is 3 and has 1 in edges. 1 out edges.
Node is 4 and has 1 in edges. 0 out edges.
Node is 2 and has 1 in edges. 0 out edges.
Node is 5 and has 0 in edges. 1 out edges.
Node is 6 and has 1 in edges. 0 out edges.
==========visit nodes post order==========
Node is 3 and has 1 in edges. 1 out edges.
Node is 4 and has 1 in edges. 0 out edges.
Node is 6 and has 1 in edges. 0 out edges.
```


