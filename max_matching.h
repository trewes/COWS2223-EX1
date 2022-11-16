#ifndef SRC_MAX_MATCHING_H
#define SRC_MAX_MATCHING_H

#include "graph.hpp"

#include <vector>


ED::Graph greedy_matching(const ED::Graph &input_graph);



class Edmonds{
public:
    Edmonds(const ED::Graph &input_graph);

    ED::Graph max_cardinality_matching();
private:

    ED::Graph graph;

    std::vector<ED::NodeId> mu, phi, rho = std::vector<ED::NodeId>{};
    std::vector<bool> scanned = std::vector<bool>{};

    bool is_outer(const ED::NodeId &vertex);
    bool is_inner(const ED::NodeId &vertex);
    bool is_oof(const ED::NodeId &vertex);

    ED::NodeId find_unscanned_outer_vertex();
    ED::NodeId neighbour_search(ED::NodeId vertex);
    std::vector<ED::NodeId> maximal_sequence_path(ED::NodeId vertex);

    void greedy_matching_mu();
};

std::vector<ED::NodeId> vector_intersection(const std::vector<ED::NodeId> &v1,
                                      const std::vector<ED::NodeId> &v2);

bool is_matching(const ED::Graph &input_graph);


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
