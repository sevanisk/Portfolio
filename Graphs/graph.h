#pragma once
#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>
#include <iostream>
#include <list>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class Graph {
    public:
        // vertex id, map of <adjacent vertex, weight to that vertex>
        unordered_map<size_t, unordered_map<size_t, double>> g;
        size_t v_num;
        size_t e_num;
        Graph(): g({}), v_num(0), e_num(0){ }

        Graph(const Graph& obj): g(obj.g), v_num(obj.v_num), e_num(obj.e_num){ }

        Graph& operator=(const Graph& obj){
            g = obj.g;
            v_num = obj.v_num;
            e_num = obj.e_num;
            return *this;
        }

        ~Graph(){
            v_num = 0;
            e_num = 0;
            g.clear();
        }

        size_t vertex_count() const{
            return v_num;
        }

        size_t edge_count() const{
            return e_num;
        }

        bool contains_vertex(size_t id) const{
            if(g.find(id) != g.end()){
                return true;
            }
            return false;
        }

        bool contains_edge(size_t u, size_t v) const{
            if(contains_vertex(u) && contains_vertex(v)){
                if(g.at(u).find(v) != g.at(u).end()){
                    return true;
                }
            }
            return false;
        }

        double cost(size_t u, size_t v) const{
            if(!contains_edge(u, v)){
                return INFINITY;
            }
            return g.at(u).at(v);
        }

        bool add_vertex(size_t id){
            if(contains_vertex(id)){
                return false;
            }
            v_num++;
            g.insert({id,{}});
            return true;
        }

        bool add_edge(size_t u, size_t v, double weight=1){
            if(!contains_vertex(u) || !contains_vertex(v) || contains_edge(u, v)){
                return false;
            }
            e_num++;
            g[u].insert({v, weight});
            g[v].insert({u, weight});
            return true;
        }

        bool remove_edge(size_t u, size_t v){
            if(!contains_edge(u , v)){
                return false;
            }
            g[u].erase(v);
            g[v].erase(u);
            e_num--;
            return true;
        }

        bool remove_vertex(size_t id){
            if(!contains_vertex(id)){
                return false;
            }
            g.erase(id);

            unordered_map<size_t, unordered_map<size_t, double>>::iterator it;
            for(auto i : g){
                if(contains_edge(i.first, id)){
                    remove_edge(i.first, id);
                }
            }
            v_num--;
            return true;
        }

        void print_minimum_spanning_tree(std::ostream& os = std::cout) const{
            if(g.empty()){
                return;
            }
            list<pair<size_t,size_t>> prim_mst = prim();
            for(auto i : prim_mst){
                os << i.first << " --{" << cost(i.first, i.second) << "} " << i.second << endl;
            }
        }

        double distance(size_t id) const{
            if(!contains_vertex(id)){
                return INFINITY;
            }
            list<pair<size_t,size_t>> prim_mst = prim();
            for(auto i : prim_mst){
                if(i.first == id || i.second == id){
                    return cost(i.first, i.second);
                }
            }
            return INFINITY;
        }

        list<pair<size_t,size_t>> prim() const{            
            list<size_t> unvisited;
            list<size_t> visited;
            list<pair<size_t, size_t>> edges;

            if(g.empty()){
                return edges;
            }


            // fills unvisited set
            for(auto i : g){
                unvisited.push_back(i.first);
            }

            cout << endl;
            visited.push_back(unvisited.front());
            unvisited.pop_front();

            while(!unvisited.empty()){
                double min = INFINITY;
                size_t start = 0;
                size_t end = 0;
                for(size_t i : visited){
                    for (size_t j : unvisited) {
                      if (g.at(i).find(j) != g.at(i).end()) {  // there is an edge
                        if (min > g.at(i).at(j)) {
                            min = g.at(i).at(j);
                            start = i;
                            end = j;
                        }
                      }
                    }

                }
                if(min == INFINITY){
                    edges.clear();
                    return edges;
                }
                unvisited.remove(end);
                visited.push_back(end);
                edges.push_back(pair<size_t, size_t> (start, end));
            }
            return edges;
        }

        void print_graph(){
            cout << "graph : ";
            for( auto i : g){
                cout << i.first << ": [";
                for(auto j : i.second){
                    cout << j.first << ": " << j.second << "  ";
                }
                cout << "]" << endl;
            }
        }

};
