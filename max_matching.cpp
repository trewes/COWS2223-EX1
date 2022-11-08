#include "max_matching.h"

#include <numeric>




ED::Graph greedy_matching(const ED::Graph &input_graph){

    ED::Graph greedy_matching_as_graph{input_graph.num_nodes()};
    for (ED::NodeId node_id = 0; node_id < input_graph.num_nodes(); ++node_id)
    {
        if (greedy_matching_as_graph.node(node_id).neighbors().empty())
        {
            for (ED::NodeId neighbor_id : input_graph.node(node_id).neighbors())
            {
                if (greedy_matching_as_graph.node(neighbor_id).neighbors().empty())
                {
                    greedy_matching_as_graph.add_edge(node_id, neighbor_id);
                    break; // Do not add more edges incident to this node!
                }
            }
        }
    }
    return greedy_matching_as_graph;
}


Edmonds::Edmonds(const ED::Graph &input_graph)
    : graph{input_graph}
    , mu(input_graph.num_nodes())
    , phi(input_graph.num_nodes())
    , rho(input_graph.num_nodes())
    , scanned(input_graph.num_nodes()) //initialised to false
    {
    //still need to initialize the vectors mu, phi and rho
    std::iota(mu.begin(), mu.end(), 0);
    std::iota(phi.begin(), phi.end(), 0);
    std::iota(rho.begin(), rho.end(), 0);

    }






ED::Graph Edmonds::max_cardinality_matching(const ED::Graph &input_graph){




    return ED::Graph(input_graph.num_nodes());
}

































