#include <iostream>
#include "graph.h"
struct NodeVisitor {
    void operator()(Graph<int, int>::NodeIterator node,
                    Graph<int, int>::SourceIterator in_begin,
                    Graph<int, int>::SourceIterator in_end,
                    Graph<int, int>::SinkIterator out_begin,
                    Graph<int, int>::SinkIterator out_end) const {
        std::cout << "Node is " << *node << " and has "
                  << std::distance(in_begin, in_end) << " in edges. "
                  << std::distance(out_begin, out_end) << " out edges. "
                  << std::endl;
    }
};
int main() {
    Graph<int, int> graph;
    auto node1 = graph.AddNode(1);
    auto node2 = graph.AddNode(2);
    auto node3 = graph.AddNode(3);
    auto node4 = graph.AddNode(4);

    auto edge1 = graph.AddEdge(node1.first, node2.first);
    auto edge2 = graph.AddEdge(node1.first, node3.first);
    auto edge3 = graph.AddEdge(node2.first, node4.first);
    std::cout << "===========================================\n";
    graph.VisitNodesDepthfirstPostorder({node1.first}, NodeVisitor{});
    std::cout << "===========================================\n";
    std::cout << graph.StringfyNodes() << std::endl;
    std::cout << graph.StringfyEdges() << std::endl;
    std::cout << graph.StringfyAdjList() << std::endl;
    std::cout << "===========================================\n";
    graph.RemoveNode(node2.first);
    std::cout << graph.StringfyEdges() << std::endl;
    std::cout << graph.StringfyAdjList() << std::endl;
    return 0;
}
