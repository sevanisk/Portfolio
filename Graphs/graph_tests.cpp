#include "graph.h"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>
using namespace std;

int main() {
    Graph G;
    G.print_graph();
    cout << endl;
    cout << endl;
    for (size_t n = 1; n <= 6; n++) {
    G.add_vertex(n);
    }

    G.print_graph();
    std::cout << "vertex count: " << G.vertex_count() << std::endl;
    std::cout << "contains 1: " << G.contains_vertex(1) << std::endl;
    G.add_edge(1,2,5);
    G.add_edge(2,3,4);
    G.add_edge(3,4,3);
    G.add_edge(4,5,2);
    G.add_edge(2,4,1);
    G.add_edge(6,3,1); 

    G.print_graph();
    std::cout << "edge count: " << G.edge_count() << std::endl;
    std::cout << "contains 1--6: " << G.contains_edge(1,2) << std::endl;
    std::cout << "cost 1--6: " << G.cost(1,2) << std::endl;
    std::cout << "vertex count: " << G.vertex_count() << std::endl;
    G.print_graph();
    G.remove_edge(2, 4);
    G.print_graph();
    G.remove_vertex(6);
    Graph h = G;
    h.add_vertex(3);
    G = h;
    cout << "here";
    G.print_minimum_spanning_tree();
    
    return 0;
}