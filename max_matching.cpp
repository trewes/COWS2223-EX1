#include "max_matching.h"

#include <numeric>
#include <algorithm>
#include <set>




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



ED::NodeId Edmonds::find_unscanned_outer_vertex() {
    //iterate over all vertices and find one with the desired property in range 0..n-1
    for (ED::NodeId x = 0; x < graph.num_nodes(); ++x) {
        if( (not scanned[x]) and is_outer(x)){
            return x;
        }
    }
    //no vertex found return n which is out of range
    return graph.num_nodes();
}

void Edmonds::neighbour_search(const ED::NodeId &vertex,
                               std::vector<ED::NodeId> &oof_neighbors,
                               std::vector<ED::NodeId> &outer_neighbors) {
    for (ED::NodeId const &neighbor: graph.node(vertex).neighbors()) {
        if( is_oof(neighbor)){
            oof_neighbors.push_back(neighbor);
        }
        else if((is_outer(neighbor) and rho[neighbor] != rho[vertex])){
            outer_neighbors.push_back(neighbor);
        }
    }

}


//checks if vertex t is already on path
bool is_already_on_path(const std::vector<ED::NodeId> &path, const unsigned int &t){
    for(int i = int(path.size())-1; i >= 0; i--){
        if(path[i] == t){
            return true;
        }
    }
    return false;
}


std::vector<ED::NodeId> Edmonds::maximal_sequence_path(const unsigned int &vertex) {
    std::vector<ED::NodeId> path{vertex};

    //have shown in lecture that the sequence eventually terminated
    //but it potentially loops and we have to check for a path
    ED::NodeId t;
    while(true){
        t = mu[path.back()];
        if(is_already_on_path(path, t)){
            break;
        }
        else {
            path.push_back(t);
        }

        t = phi[path.back()];
        if(is_already_on_path(path, t)){
            break;
        }
        else {
            path.push_back(t);
        }
    }


    return path;
}



std::vector<ED::NodeId> vector_intersection(const std::vector<ED::NodeId> &v1,
                                     const std::vector<ED::NodeId> &v2){
    std::vector<ED::NodeId> intersection{};

    // Creates an empty hashset
    std::set<ED::NodeId> hash_set;

    // Traverse the first vector and store its elements in hashset
    for (const ED::NodeId & i : v1){
        hash_set.insert(i);
    }

    // go over second vector and check if an element is in hashset
    //if so, add it to the intersection
    for (const ED::NodeId & j : v2){
        if (hash_set.count(j)){
            intersection.push_back(j);
        }
    }
    return intersection;
}


void Edmonds::greedy_matching_mu(){

    for (ED::NodeId node_id = 0; node_id < graph.num_nodes(); ++node_id){
        if (mu[node_id] == node_id){
            for (ED::NodeId neighbor_id : graph.node(node_id).neighbors()){
                //if both vertices are unmatched and neighbours, add matching edge
                if (mu[neighbor_id] == neighbor_id){
                    mu[node_id] = neighbor_id;
                    mu[neighbor_id] = node_id;
                    break;
                }
            }
        }
    }
}

ED::Graph Edmonds::max_cardinality_matching() {

    //find greedy/heuristic matching
    greedy_matching_mu();

    // while we find an outer vertex x that has scanned(x) == false
    //we iterate over the steps of tha algorithm, otherwise stop
    ED::NodeId x;
    std::vector<ED::NodeId> oof_neighbors = {};
    std::vector<ED::NodeId> outer_neighbors = {};
    //OUTER VERTEX SCAN
    while(x = find_unscanned_outer_vertex(), x!= graph.num_nodes()){

        //NEIGHBOUR SEARCH
        oof_neighbors = {};
        outer_neighbors = {};
        neighbour_search(x, oof_neighbors, outer_neighbors);
        //did not find a neighbour with the right property, set scanned(X)=true and go to outer vertex scan
        if(oof_neighbors.empty() and outer_neighbors.empty()){
            scanned[x] = true;
            continue;
        }

        //GROW
        //add all out of forest neighbours of x, all at once
        for(ED::NodeId y : oof_neighbors){
            //check if it is still out of forest after changing phi in
            //possible earlier iterations of the grow step
            //only this condition could change in oof characterization
            if( phi[mu[y]]  == mu[y]){
                phi[y] = x;
            }
        }

        //now either augment or shrink on the rest of the vertices

        std::vector<ED::NodeId> P_x = maximal_sequence_path(x);
        for(ED::NodeId y : outer_neighbors){
            //find path from y and the intersection of the two paths
            std::vector<ED::NodeId> P_y = maximal_sequence_path(y);
            std::vector<ED::NodeId> intersection = vector_intersection(P_x, P_y);

            //AUGMENT
            if(intersection.empty()){ //paths are disjoint
                //change mu and phi for all vertices on paths with odd distance from x/y
                for (unsigned int i = 0; 2*i + 1 < P_x.size(); ++i) {
                    ED::NodeId v = P_x[2*i + 1];
                    mu[phi[v]] = v;
                    mu[v] =  phi[v];
                }//I don't think it matters that a node may be on both parts
                for (unsigned int i = 0; 2*i + 1 < P_y.size(); ++i) {
                    ED::NodeId v = P_y[2*i + 1];
                    mu[phi[v]] = v;
                    mu[v] =  phi[v];
                }
                //change mu and phi for x and y
                mu[x] = y;
                mu[y] = x;
                //reset scanned with default value false, phi and rho
                scanned = std::vector<bool>(graph.num_nodes());
                std::iota(phi.begin(), phi.end(), 0);
                std::iota(rho.begin(), rho.end(), 0);

                //we had an iteration where we augmented and reset forest.
                //start again from outer vertex scan
                break;
            }
                //SHRINK
            else{
                //find first vertex r in intersection with rho(r)=r
                //
                auto r_iterator = std::find_if(intersection.begin(), intersection.end(),
                                               [&](ED::NodeId r){return rho[r] == r;});
                if(r_iterator == intersection.end()){
                    throw std::runtime_error("There is no vertex in the intersection of the Paths"
                                             " such that rho(r)==r.");
                }
                ED::NodeId r = *r_iterator;
                //for v on path P_x,r resp. P_y,r with odd distance from x resp. y, change phi
                auto P_x_iterator_until_r = std::find(P_x.begin(), P_x.end(), r);
                auto P_y_iterator_until_r = std::find(P_y.begin(), P_y.end(), r);
                if(P_x_iterator_until_r == P_x.end() or P_y_iterator_until_r == P_y.end()){
                    throw std::runtime_error("Could not find element r in one of the paths.");
                }
                unsigned int index_r_on_P_x = std::distance(P_x.begin(), P_x_iterator_until_r);
                unsigned int index_r_on_P_y = std::distance(P_y.begin(), P_y_iterator_until_r);

                //loop over path until r but only take every odd distance vertex
                for (unsigned int i = 0; 2*i + 1 <= index_r_on_P_x ; ++i) {
                    ED::NodeId vertex = P_x[2*i + 1];
                    if(rho[phi[vertex]] != r){
                        phi[phi[vertex]] = vertex;
                    }
                }
                for (unsigned int i = 0; 2*i + 1 <= index_r_on_P_y; ++i) {
                    ED::NodeId vertex = P_y[2*i + 1];
                    if(rho[phi[vertex]] != r){
                        phi[phi[vertex]] = vertex;
                    }
                }
                //change phi for x and y
                if(rho[x] != r){
                    phi[x] = y;
                }
                if(rho[y] != r){
                    phi[y] = x;
                }
                //change rho for vertices whose rho is in union of partial paths
                for (ED::NodeId v = 0; v < graph.num_nodes(); ++v) {

                    //see if rho(v) is in path of x until r
                    if(std::find(P_x.begin(), P_x.end(), rho[v]) <= P_x_iterator_until_r){
                        rho[v]= r;
                    }
                    if(std::find(P_y.begin(), P_y.end(), rho[v]) <= P_y_iterator_until_r){
                        rho[v]= r;
                    }
                }
            }
            //finished this vertex, go to next neighbor y of x
        }
        //finished all neighbors of y or has augmented, go back to outer vertex scan
    }
    //finished all unscanned outer vertices, algorithm is done


    //convert matching from mu to a subgraph of original graph
    ED::Graph matching_as_graph(graph.num_nodes());
    for (ED::NodeId v = 0; v < graph.num_nodes(); ++v) {
        if( mu[v] != v and v < mu[v]){//check if matching edge and add only when iterating over smaller vertex
            matching_as_graph.add_edge(v, mu[v]);
        }
    }

    return matching_as_graph;
}



bool is_matching(const ED::Graph &input_graph){
    for (ED::NodeId node_id = 0; node_id < input_graph.num_nodes(); ++node_id){
        if ( input_graph.node(node_id).neighbors().size() > 1){
            return false;
        }
    }
    return true;
}

































