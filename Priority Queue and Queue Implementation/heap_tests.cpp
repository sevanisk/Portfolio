#include <functional>
#include <deque>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "heap.h"
using namespace std;

int main() {
    // TODO(student): write at least 1000 lines of test
    vector<int> heap{150,80,40,30,10,70,110,100,20,90,60,50,120,140,130};
    for (int j : heap) { std::cout << j << "    "; }
        cout << endl;
    for(int k = 1; k < 16; k++){
        cout << k << "    ";
    }
    cout << endl;
    heapify(&heap);
    for (int i : heap) { std::cout << i << " "; }
    heap_get_min(heap);

    for (unsigned j = 0; j < 4; j++) {
    std::cout << "minimum is " << heap_get_min(heap) << std::endl;
    
    std::cout << "delete min" << std::endl;
    heap_delete_min(&heap);
    
    std::cout << "heap: ";
    for (int i : heap) { std::cout << i << " "; }
    std::cout << std::endl;
}


    
    int val = 1;
    heap_insert(&heap, val);
    for (int l : heap) { std::cout << l << "    "; }
    cout << endl;
    return 0;
}
