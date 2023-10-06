#ifndef STATE_H
#define STATE_H
#include <iostream>
#include <vector>

using namespace std;

class State{
private:
    vector<vector<char>> state { { } };

public:
    // constructors
    State();
    State(vector<vector<char>> state);  
    State(int stacks);

    // copy constructor (i hate c++ where is my automatic garbage collection D: 
    State(const State &s);

    // moves the top block in start_stack row to the top of goal_stack row
    void moveBlock(int start_stack, int goal_stack);

    // adds a letter specified in block to the top of the row stack
    void addBlock(int stack, char block);

    // prints the state for the output and for testing
    // prints in this format:
    //
    // A B
    // C
    // DE
    //
    // where A and B are in the same "stack" and B is the top of the stack
    // A B is one row, C is another row, D E is the last row
    // B, C, and E are the top of their respective stack/row
    void printState();

    // gets the vector of vectors of char that holds the state
    // this is mostly just used for printing the state, the string rep is usually used for the heuristic
    vector<vector<char>> getState() const;

    // checks if the row is empty i.e. if there is a block on top
    bool blockOnTop(int row);

    // gets the string representation of the vector of vectors of chars
    // stacks are separated by a *
    // example: AB*C*DE*
    string getStringRep();

};

#endif