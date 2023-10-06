#pragma once

#include <functional>
#include <functional>
#include <deque>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
using namespace std;


template <class Container, class Compare=less<typename Container::value_type>>
void heapmaker(Container* h, Compare comp= less<typename Container::value_type>{}, size_t i=0){
	size_t smallest = i;
	size_t leftchild = (i * 2);
	size_t rightchild = (i * 2) + 1;
	if(leftchild < h->size() && comp(h->at(leftchild), h->at(i))){
		smallest = leftchild;
	}

	if(rightchild < h->size() && comp(h->at(rightchild), h->at(i)) && comp(h->at(rightchild), h->at(leftchild))){
		smallest = rightchild;
	}

	if(smallest != i){
		swap(h->at(i), h->at(smallest));
		heapmaker(h, comp, smallest);
	}
}


template <class Container, class Compare=less<typename Container::value_type>>
void heapify(Container* h, Compare comp= less<typename Container::value_type>{}){
	auto it = h->begin();
    it = h->insert(it, typename Container::value_type{});
	for(size_t i = h->size()-1; i > 0; i--){
		heapmaker(h, comp, i);
	}
}

size_t parent(size_t current){
	if(current % 2 == 0){
		return current/2;
	}
	return (current-1)/2;
}

template <class Container, class Compare=std::less<typename Container::value_type>>
void heap_insert(Container* cont, const typename Container::value_type& val, Compare comp=std::less<typename Container::value_type>{}){
	if(cont->size() == 0){
		return;
	}
	cont->push_back(val);
	size_t par = parent(cont->size()-1);
	size_t curr = cont->size() - 1;
	while(par > 0 && comp(val, cont->at(par))){
		swap(cont->at(curr), cont->at(par));
		curr = par;
		par = parent(curr);
	}

}

template <class Container>
const typename Container::value_type& heap_get_min(const Container& cont){
	if(cont.size() <= 1){
		throw std::invalid_argument("L");
	}
	return cont[1];
}

template <class Container, class Compare=std::less<typename Container::value_type>>
void heap_delete_min(Container* cont, Compare comp=std::less<typename Container::value_type>{}){
	if(cont->size() <= 1){
		throw std::invalid_argument("L");
	}
	swap(cont->at(1), cont->at(cont->size()-1));
	cont->pop_back();
	size_t curr = 1;
	while(curr < cont->size()){
		size_t right = curr*2 + 1;
		size_t left = curr*2;
		if(right < cont->size() && comp(cont->at(right), cont->at(curr)) && comp(cont->at(right), cont->at(left))){
			swap(cont->at(curr), cont->at(right));
			curr = curr*2 + 1;
		}
		else if(left < cont->size() && comp(cont->at(left), cont->at(curr))){
			swap(cont->at(curr), cont->at(left));
			curr *= 2;
		}
		else{
			break;
		}
	}
}
