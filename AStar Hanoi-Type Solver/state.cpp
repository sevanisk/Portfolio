#include <iostream>
#include "state.h"

using namespace std;


State::State(): state({{}}){ }


State::State(vector<vector<char>> s): state(s){ }


State::State(int stacks): state(vector<vector<char>>{}){
    for(int i = 0; i < stacks; i++){
        vector<char> new_stack_row = {};
        state.push_back(new_stack_row);
    }
}


// copy constructor
State::State(const State &s) {
    state = s.getState();
}


void State::moveBlock(int start_stack, int goal_stack){
    char moveletter = state.at(start_stack).back();
    state.at(start_stack).pop_back();
    state.at(goal_stack).push_back(moveletter);
}


void State::addBlock(int stack, char block){
    state.at(stack).push_back(block);
}


void State::printState(){
    for(int row = 0; row < state.size(); row++){
        for(int col = 0; col < state.at(row).size(); col++){
            cout << state.at(row).at(col) << " ";
        }
        cout << endl;
    }
}


vector<vector<char>> State::getState() const{
    return state;
}


bool State::blockOnTop(int row){
    return !state.at(row).empty();
}


string State::getStringRep(){
    string stringrep = "";
    for(int row = 0; row < state.size(); row++){
        for(int col = 0; col < state.at(row).size(); col++){
            stringrep += state.at(row).at(col);
        }
        stringrep += "*";
    }
    return stringrep;
}

