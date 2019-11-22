#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
template <typename NodeTy = int, typename EdgeTy = int>
class Graph {
public:
    using Node = NodeTy;
    using NodeSet = std::unordered_set<Node>;
    using NodePtr = const NodeTy*;
    using NodeRef = NodeTy&;
    using NodeIterator = typename NodeSet::iterator;
    using ConstNodeIterator = typename NodeSet::const_iterator;
    using Edge = std::tuple<NodePtr, NodePtr, EdgeTy>;
    using EdgeList = std::list<Edge>;
    using EdgeIterator = typename EdgeList::iterator;
    using ConstEdgeIterator = typename EdgeList::const_iterator;
    using AdjNodes = std::vector<NodeIterator>;
    using AdjList = std::unordered_map<NodePtr, AdjNodes>;

    using SourceIterator = typename std::vector<NodeIterator>::iterator;
    using SinkIterator = typename std::vector<NodeIterator>::iterator;

public:
    Graph() = default;
    Graph(const Graph<NodeTy, EdgeTy>&);
    Graph& operator=(const Graph<NodeTy, EdgeTy>&);
    Graph(Graph<NodeTy, EdgeTy>&&);

    NodeIterator begin();
    NodeIterator end();
    ConstNodeIterator begin() const;
    ConstNodeIterator end() const;

    std::pair<NodeIterator, bool> AddNode(const NodeTy&);
    std::pair<EdgeIterator, bool> AddEdge(ConstNodeIterator, ConstNodeIterator,
                                          const EdgeTy&);
    std::pair<EdgeIterator, bool> AddEdge(NodeIterator, NodeIterator,
                                          const EdgeTy&);
    std::pair<EdgeIterator, bool> AddEdge(NodeIterator, NodeIterator);
    std::pair<EdgeIterator, bool> AddEdge(const NodeTy&, const NodeTy&);
    NodeIterator RemoveNode(ConstNodeIterator);
    // NodeIterator RemoveNode(const NodeTy&);
    EdgeIterator RemoveEdge(ConstEdgeIterator);
    void Clear();
    template <typename Func>
    void VisitNodesDepthfirstPostorder(std::vector<NodeIterator> heads,
                                       const Func& visitor);
    std::string StringfyNodes();
    std::string StringfyEdges();
    std::string StringfyAdjList();

private:
    NodeSet nodes_;
    EdgeList edges_;
    // if a->b in edges_, then a in adjlist_precede_[b]
    AdjList adjlist_precede_;
    // if a->b in edges_, then b in adjlist_succeed_[a]
    AdjList adjlist_succeed_;
};

template <typename NodeTy, typename EdgeTy>
typename Graph<NodeTy, EdgeTy>::NodeIterator Graph<NodeTy, EdgeTy>::begin() {
    return nodes_.begin();
}

template <typename NodeTy, typename EdgeTy>
typename Graph<NodeTy, EdgeTy>::NodeIterator Graph<NodeTy, EdgeTy>::end() {
    return nodes_.end();
}
template <typename NodeTy, typename EdgeTy>
std::pair<typename Graph<NodeTy, EdgeTy>::NodeIterator, bool>
Graph<NodeTy, EdgeTy>::AddNode(const NodeTy& node) {
    return nodes_.insert(node);
}

template <typename NodeTy, typename EdgeTy>
std::pair<typename Graph<NodeTy, EdgeTy>::EdgeIterator, bool>
Graph<NodeTy, EdgeTy>::AddEdge(NodeIterator from, NodeIterator to,
                               const EdgeTy& value) {
    if (std::addressof(*from) == std::addressof(*to)) {
        throw std::runtime_error("Cannot add_edge linking a node to itself");
    }
    auto found_iter = std::find_if(
        edges_.begin(), edges_.end(),
        [from, to](const Graph<NodeTy, EdgeTy>::Edge& edge) -> bool {
            return std::get<0>(edge) == std::addressof(*from) &&
                   std::get<1>(edge) == std::addressof(*to);
        });
    if (found_iter != edges_.end()) {
        return std::make_pair(found_iter, true);
    } else {
        adjlist_precede_[std::addressof(*to)].emplace_back(from);
        adjlist_succeed_[std::addressof(*from)].emplace_back(to);
        auto&& iter = edges_.emplace(
            edges_.begin(),
            std::make_tuple(std::addressof(*from), std::addressof(*to), value));
        return std::make_pair(iter, false);
    }
}

template <typename NodeTy, typename EdgeTy>
std::pair<typename Graph<NodeTy, EdgeTy>::EdgeIterator, bool>
Graph<NodeTy, EdgeTy>::AddEdge(NodeIterator from, NodeIterator to) {
    return AddEdge(from, to, EdgeTy());
}

template <typename NodeTy, typename EdgeTy>
typename Graph<NodeTy, EdgeTy>::NodeIterator Graph<NodeTy, EdgeTy>::RemoveNode(
    ConstNodeIterator node_to_remove_iter) {
    auto node_after_remove_iter = nodes_.erase(node_to_remove_iter);
    auto found_iter =
        adjlist_succeed_.find(std::addressof(*node_to_remove_iter));
    if (found_iter != adjlist_succeed_.end()) {
        adjlist_succeed_.erase(found_iter);
    }
    found_iter = adjlist_precede_.find(std::addressof(*node_to_remove_iter));
    if (found_iter != adjlist_precede_.end()) {
        adjlist_precede_.erase(found_iter);
    }
    std::vector<NodePtr> nodes_precedes_this_node, nodes_succeeds_this_node;
    for (auto&& iter = edges_.begin(); iter != edges_.end(); iter++) {
        if (std::get<1>(*iter) == std::addressof(*node_to_remove_iter)) {
            nodes_precedes_this_node.emplace_back(std::get<0>(*iter));
        } else if (std::get<0>(*iter) == std::addressof(*node_to_remove_iter)) {
            nodes_succeeds_this_node.emplace_back(std::get<1>(*iter));
        }
    }
    for (auto&& node_precedes_this_node : nodes_precedes_this_node) {
        auto& adjnodes_to = adjlist_succeed_[node_precedes_this_node];
        adjnodes_to.erase(
            std::find_if(adjnodes_to.begin(), adjnodes_to.end(),
                         [node_to_remove_iter](const NodeIterator& iter) {
                             return *node_to_remove_iter == *iter;
                         }));
    }
    for (auto&& node_succeeds_this_node : nodes_succeeds_this_node) {
        auto& adjnodes_from = adjlist_precede_[node_succeeds_this_node];
        adjnodes_from.erase(
            std::find_if(adjnodes_from.begin(), adjnodes_from.end(),
                         [node_to_remove_iter](const NodeIterator& iter) {
                             return *node_to_remove_iter == *iter;
                         }));
    }

    edges_.erase(
        std::remove_if(edges_.begin(), edges_.end(),
                       [node_to_remove_iter](
                           const typename Graph<NodeTy, EdgeTy>::Edge& edge) {
                           return std::get<0>(edge) ==
                                      std::addressof(*node_to_remove_iter) ||
                                  std::get<1>(edge) ==
                                      std::addressof(*node_to_remove_iter);
                       }),
        edges_.end());
    return node_after_remove_iter;
}

template <typename NodeTy, typename EdgeTy>
typename Graph<NodeTy, EdgeTy>::EdgeIterator Graph<NodeTy, EdgeTy>::RemoveEdge(
    ConstEdgeIterator edge_to_remove_iter) {
    auto& adjnodes_from_source_node =
        adjlist_succeed_[std::get<0>(edge_to_remove_iter)];
    adjnodes_from_source_node.erase(std::find(
        adjnodes_from_source_node.begin(), adjnodes_from_source_node.end(),
        std::get<1>(edge_to_remove_iter)));
    auto& adjnodes_to_dest_node =
        adjlist_precede_[std::get<1>(edge_to_remove_iter)];
    adjnodes_to_dest_node.erase(std::find(adjnodes_to_dest_node.begin(),
                                          adjnodes_to_dest_node.end(),
                                          std::get<1>(edge_to_remove_iter)));

    return edges_.erase(edge_to_remove_iter);
}
// post order node visit functions
template <typename NodeTy, typename EdgeTy>
template <typename Func>
void Graph<NodeTy, EdgeTy>::VisitNodesDepthfirstPostorder(
    std::vector<NodeIterator> heads, const Func& visitor) {
    for (auto&& head : heads) {
        std::unordered_set<NodePtr> visited_nodes;
        std::stack<NodeIterator> nodes_to_visit;

        nodes_to_visit.emplace(head);
        while (!nodes_to_visit.empty()) {
            auto&& current_node = nodes_to_visit.top();
            nodes_to_visit.pop();

            if (visited_nodes.find(std::addressof(*current_node)) ==
                visited_nodes.end()) {
                auto&& adjnodes_to_this_node =
                    adjlist_precede_[std::addressof(*current_node)];
                auto&& adjnodes_from_this_node =
                    adjlist_succeed_[std::addressof(*current_node)];

                visitor(current_node, adjnodes_to_this_node.begin(),
                        adjnodes_to_this_node.end(),
                        adjnodes_from_this_node.begin(),
                        adjnodes_from_this_node.end());
                visited_nodes.insert(std::addressof(*current_node));
                for (auto&& to_node_iter = adjnodes_from_this_node.begin();
                     to_node_iter != adjnodes_from_this_node.end();
                     to_node_iter++) {
                    // double check, not neccessary, but for performance,
                    // guarantee there is not too many nodes in nodes_to_visit
                    if (visited_nodes.find(std::addressof(*(*to_node_iter))) ==
                        visited_nodes.end()) {
                        nodes_to_visit.emplace(*to_node_iter);
                    }
                }
            }
        }
    }
}
template <typename NodeTy, typename EdgeTy>
std::string Graph<NodeTy, EdgeTy>::StringfyNodes() {
    std::ostringstream nodes_str_buf;
    for (auto&& node : nodes_) {
        nodes_str_buf << std::to_string(node);
        nodes_str_buf << '\t';
    }
    return nodes_str_buf.str();
}
template <typename NodeTy, typename EdgeTy>
std::string Graph<NodeTy, EdgeTy>::StringfyEdges() {
    std::ostringstream edges_str_buf;
    for (auto&& edge : edges_) {
        edges_str_buf << std::to_string(*std::get<0>(edge));
        edges_str_buf << " -> ";
        edges_str_buf << std::to_string(*std::get<1>(edge));
        edges_str_buf << '\t';
    }
    return edges_str_buf.str();
}
template <typename NodeTy, typename EdgeTy>
std::string Graph<NodeTy, EdgeTy>::StringfyAdjList() {
    std::ostringstream adjlist_str_buf;
    adjlist_str_buf << "================succeed================\n";
    for (auto&& adjnodes : adjlist_succeed_) {
        adjlist_str_buf << std::to_string(*adjnodes.first);
        adjlist_str_buf << " : ";
        for (auto&& neighbor : adjnodes.second) {
            adjlist_str_buf << std::to_string(*neighbor);
            adjlist_str_buf << " ";
        }
        adjlist_str_buf << '\n';
    }
    adjlist_str_buf << "================precede================\n";
    for (auto&& adjnodes : adjlist_precede_) {
        adjlist_str_buf << std::to_string(*adjnodes.first);
        adjlist_str_buf << " : ";
        for (auto&& neighbor : adjnodes.second) {
            adjlist_str_buf << std::to_string(*neighbor);
            adjlist_str_buf << " ";
        }
        adjlist_str_buf << '\n';
    }

    return adjlist_str_buf.str();
}
