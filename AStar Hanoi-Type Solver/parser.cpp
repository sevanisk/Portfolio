#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include "parser.h"

using namespace std;

Parser::Parser(): filename(""), stacks(0), blocks(0), start(nullptr), goal(nullptr){  }

Parser::Parser(string filename): start(new State()), goal(new State()), stacks(0), blocks(0){
    readFile(filename);
}

Parser::~Parser(){
    delete start;
    delete goal;
}

void Parser::readFile(string filename){
    ifstream inFS;
    inFS.open(filename);

    // file cant be opened
    if(!inFS.is_open()){
        cout << "COULDN'T OPEN FILE";
    }

    // get the stacks and blocks
    string firstLine = "";
    getline(inFS, firstLine);

    istringstream inSS(firstLine);

    inSS >> stacks;
    inSS >> blocks;

    // create the empty start and goal state
    start = new State(stacks);
    goal = new State(stacks);
    char letter;
    string currLine = "";

    // throw away line of >>>>
    getline(inFS, firstLine);

    for(int i = 0; i < stacks; i++){
        getline(inFS, currLine);

        if(currLine.find(">") != string::npos){
            break;
        }

        while(currLine != ""){
            letter = currLine.at(0);
            currLine = currLine.substr(1);
            start->addBlock(i, letter);
        }
    }

    // throw away line of >>>>
    getline(inFS, currLine);

    for(int i = 0; i < stacks; i++){
        getline(inFS, currLine);
        

        if(currLine.find(">") != string::npos){
            break;
        }
        
        while(currLine != ""){
            letter = currLine.at(0);
            currLine = currLine.substr(1);
            goal->addBlock(i, letter);

        }
        
    }

    inFS.close();
}

State* Parser::getStart(){
    return start;
}

State* Parser::getGoal(){
    return goal;
}

int Parser::getStacks(){
    return stacks;
}

int Parser::getBlocks(){
    return blocks;
}
