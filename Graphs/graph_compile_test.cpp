#include <iostream>
#include <sstream>
#include "graph.h"

int main() {
    Graph G;
    G.add_vertex(3);
    G.remove_vertex(3);

    G.print_minimum_spanning_tree();
}
