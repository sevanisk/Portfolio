#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <string>
#include <vector>
#include "state.h"

using namespace std;
class Parser{
private:
    string filename;
    int stacks, blocks;
    State* start;
    State* goal;
public:
    // constructors
    Parser();
    Parser(string filename);
    ~Parser(); // (idek if this is needed i hate c++ i hate rule of three i'm crying)

    // reads the file (IF IT IS THE SAME FORMAT AT THE GIVEN PROBLEMS)
    // creates a start and goal state that are stored WITHIN THE PARSER
    // the main then uses the accessors to get the start and goal
    void readFile(string filename);

    // accessors
    State* getStart();
    State* getGoal();
    int getStacks();
    int getBlocks();
};

#endif