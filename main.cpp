#include <iostream> // For writing to the standard output.
#include <fstream> // For reading input files.

#include "graph.hpp"
#include "max_matching.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Expected two arguments, found " << argc - 1 << std::endl;
        return EXIT_FAILURE; // return 1 would do the same, but is way too easy to mix up!
    }

    std::fstream input_file_graph{argv[2]};
    ED::Graph const graph = ED::Graph::read_dimacs(input_file_graph);

    ED::Graph greedy_matching_as_graph = greedy_matching(graph);
//    std::cout << greedy_matching_as_graph;
    std::cout << "Greedy found a matching of size " << greedy_matching_as_graph.num_edges() << std::endl;

    std::cout << "That was the result of the greedy algorithm." << std::endl
              << "Now follows the result of Edmonds algorithm." << std::endl;

    ED::Graph matching_edmonds = Edmonds(graph).max_cardinality_matching();
    std::cout << matching_edmonds;
    if(not is_matching(matching_edmonds)){
        throw std::runtime_error("The graph found by edmonds algorithm is not a matching!");
    }
    std::cout << "Found a matching of size " << matching_edmonds.num_edges() << std::endl;


    return EXIT_SUCCESS;
}

