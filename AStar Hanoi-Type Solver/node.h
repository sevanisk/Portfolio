#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <vector>
#include <map>
#include "state.h"

using namespace std;

class Node{
private:
    State* state; 
    State* goal;
    Node* parent;
    vector<Node*> children;
    int heuristic;
    int h_method;

public:
    // constructors
    // default
    Node();

    // used for the root (which has no parent)
    Node(State* root, State* goal, int h);

    // used for every other node
    Node(State* curr, State* goal, Node* par, int h);

    // finds the children (only used for the root)
    // g is the goal state
    // h is heuristic method
    vector<Node*> findChildren(State* g, int h);

    // finds the children for a node i.e. SUCCESSORS FUNCTION
    // DOES NOT ADD ANY CHILDREN ALREADY IN R (REACHED)
    // g is the goal state
    // h is heuristic method
    vector<Node*> findChildren(map<string, Node*> r, State* g, int h);

    // makes a new Node for the children
    // links the child node to its parent and adds it to the children vector
    Node* makeChild(State* s, State* g, size_t curr_row, size_t goal_row, int h);

    // printing the node and other variations
    // mostly for debugging
    void printNode();
    void printChildren();
    void printChildrenKey();

    // accessors
    string getHashKey();
    Node* getParent();
    vector<Node*> getChildren();
    int getHeuristic() const;
    State* getState();

    // HEURISTIC FUNCTIONS
    // main heuristic function
    void calculateHeuristic();

    void h1(); // just point blank checks the characters that match in the string. a very blunt force sort of algorithmic weapon
    void h2(); // separates the start and goal string into row
    void h3(); // just...does not work. this heuristic is left here as a cautionary tale to myself. it double counts the amount of moves needed for a letter
    void h4(); // best heuristic. explained in results.

    // HELPERS

    // gets the number of not matching characters between s1 and s2
    int numMismatch(string s1, string s2);

    // checks if the letter is in the same position in s1 and s2
    bool rightRow(char letter, string s1, string s2);

    // takes the full string and makes it into a vector of string rows (without the *)
    vector<string> makeRows(string s);

};

struct PQ{
    // operator overloading for p_queue
    bool operator()(Node* n1, Node* n2) 
    {
        return (*n1).getHeuristic() > (*n2).getHeuristic();
    }
};

#endif