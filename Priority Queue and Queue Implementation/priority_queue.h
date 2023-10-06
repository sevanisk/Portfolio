#pragma once

#include <functional>
#include <deque>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "heap.h"
using namespace std;

template <class Comparable, class Container=std::vector<Comparable>, class Compare=std::less<typename Container::value_type>>
class PriorityQueue {
private:
    Container container;
    Compare compare;
    size_t sz;
public:
    PriorityQueue():container(Container()), compare(Compare()), sz(0){

    }

    explicit PriorityQueue(const Compare& value): container(0), compare(value), sz(0){

    }

    explicit PriorityQueue(const Container& other): container(other), compare(Compare()), sz(other.size()){
        heapify(&container);
    }

    PriorityQueue(const Compare& value, const Container& other): container(other), compare(value), sz(other.size()){
        heapify(&container);
    }

    typename Container::const_reference top() const{
        return heap_get_min(container);
    }

    bool empty() const{
        if(container.size() <= 1 || sz == 0){
            return true;
        }
        return false;
    }

    size_t size() const{
        return sz;
    }

    void make_empty(){
        container.clear();
        sz = 0;
    }

    void push(const typename Container::value_type& val){
        sz++;
        heap_insert(&container, val, compare);
    }

    void pop(){
        sz--;
        heap_delete_min(&container, compare);
    }

    void print_queue(std::ostream& os = std::cout) const{
        if(container.size() <= 1){
            os << "<empty>\n";
            return;
        }
        for(size_t i = 1; i < container.size()-1; i++){
            os << container.at(i) << ", ";
        }
        os << container.back() << "\n";
    }
};
