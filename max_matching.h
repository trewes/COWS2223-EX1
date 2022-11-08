#ifndef SRC_MAX_MATCHING_H
#define SRC_MAX_MATCHING_H

#include "graph.hpp"

#include <vector>


ED::Graph greedy_matching(const ED::Graph &input_graph);



class Edmonds{
public:
    Edmonds(const ED::Graph &input_graph);

    ED::Graph max_cardinality_matching(const ED::Graph &input_graph);
private:

    ED::Graph graph;

    std::vector<ED::NodeId> mu, phi, rho = std::vector<ED::NodeId>{};
    std::vector<bool> scanned = std::vector<bool>{};

    bool is_outer(const ED::NodeId &vertex);
    bool is_inner(const ED::NodeId &vertex);
    bool is_oof(const ED::NodeId &vertex);
};



//BEGIN: Inline section

inline
bool Edmonds::is_outer(const ED::NodeId &vertex) {
    return (mu[vertex] == vertex) or (phi[mu[vertex]] != mu[vertex]);
}

inline
bool Edmonds::is_inner(const ED::NodeId &vertex) {
    return (phi[mu[vertex]] == mu[vertex]) and (phi[vertex] != vertex);
}

inline
bool Edmonds::is_oof(const ED::NodeId &vertex) {
    return (mu[vertex] != vertex) and (phi[vertex] == vertex)
           and (phi[mu[vertex]] == mu[vertex]);
}



#endif //SRC_MAX_MATCHING_H
