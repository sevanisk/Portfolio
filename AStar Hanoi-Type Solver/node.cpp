#include "node.h"

using namespace std;

Node::Node():state(nullptr), goal(nullptr), parent(nullptr), children({}), heuristic(-1), h_method(3){ }


Node::Node(State* root, State* goal, int h): state(root), goal(goal), parent(nullptr), children({}), heuristic(-1), h_method(3){
    h_method = h;
    calculateHeuristic();
}


Node::Node(State* curr, State* goal, Node* par, int h): state(curr), goal(goal), parent(par), children({}), heuristic(-1), h_method(3){
    h_method = h;
    calculateHeuristic();
}


vector<Node*> Node::findChildren(State* g, int h){
    for(size_t row = 0; row < state->getState().size(); row++){

        for(size_t row2 = 0; row2 < state->getState().size(); row2++){

            if(row != row2 && state->blockOnTop(row)){
                children.push_back(makeChild(state, g, row, row2, h));
            }

        }

    }

    return children;
}


vector<Node*> Node::findChildren(map<string, Node*> r, State* g, int h){
    for(size_t row = 0; row < state->getState().size(); row++){

        for(size_t row2 = 0; row2 < state->getState().size(); row2++){

            if(row != row2 && state->blockOnTop(row)){

                Node* n = makeChild(state, g, row, row2, h);

                // if the new node is NOT IN REACHED it is added to children
                if(r.count(n->getHashKey()) <= 0){
                    children.push_back(n);
                }

            }

        }

    }

    return children;
}


Node* Node::makeChild(State* s, State* g, size_t curr_row, size_t goal_row, int h){
    // makes a child and makes it different from current by just moving a block inside it
    State* child = new State(*s);
    child->moveBlock(curr_row, goal_row);
    
    /* CHECK TO MAKE SURE DEEP COPY WAS CREATED
    cout << "TEST" << endl;
    child->printState();
    s->printState();
    */

    Node* child_node = new Node(child, g, this, h);
    return child_node;
}


void Node::printNode(){
    state->printState();
}


void Node::printChildren(){
    cout << endl << endl << "Parent:" << endl;
    printNode();
    cout << endl << endl << "Children:" << endl;
    for(Node* n : children){
        n->printNode();
    }
}


void Node::printChildrenKey(){
    cout << endl << endl << "Parent:" << endl;
    cout << getHashKey() << endl;
    cout << endl << endl << "Children:" << endl;
    for(Node* n : children){
       cout <<  n->getHashKey() << endl;
    }
}


// accessors
string Node::getHashKey(){
    return state->getStringRep();
}


Node* Node::getParent(){
    return parent;
}


vector<Node*> Node::getChildren(){
    return children;
}


int Node::getHeuristic() const{
    return heuristic;
}


State* Node::getState(){
    return state;
}


void Node::calculateHeuristic(){
    // to avoid unnecessary recalculation
    if(heuristic == -1){
        switch(h_method){
            case 1:
                h1();
                break;
            case 2:
                h2();
                break;
            case 3:
                h4();
                break;
        }
    }
}


// most basic heuristic -- just what letters are mismatched in the PLAIN STRING REP
void Node::h1(){
    string curr = state->getStringRep();
    string gol = goal->getStringRep();

    int h = 0;
    h=numMismatch(curr, gol);

    heuristic = h;
}


void Node::h2(){
    string curr = state->getStringRep();
    string gol = goal->getStringRep();

    string curr_r = curr.substr(0, curr.find('*'));
    string gol_r = gol.substr(0, gol.find('*'));

    int h = 0;
    while(curr.find('*') != -1){
        
        h+= numMismatch(curr_r, gol_r);

        curr = curr.substr(curr.find('*')+1);
        gol = gol.substr(gol.find('*')+1);

        curr_r = curr.substr(0, curr.find('*'));
        gol_r = gol.substr(0, gol.find('*'));
    }

    heuristic = h;
}


// THIS DOESN'T WORK RIGHT NOW IT DOUBLE COUNTS HEURISTIC VALUES
void Node::h3(){
    string curr = state->getStringRep();
    string gol = goal->getStringRep();
    //vector<pair<char, int>> letters;
    map<char, int> letters;

    for(char c: curr){
        if(c != '*'){
            letters[c] = 0;
        }
    }

    int h = 0;

    for(auto l:letters){
        int pos = curr.find(l.first);
        int gol_pos = gol.find(l.first);
        // this is the first algorithm i thought of before i realized it ends up double (or triple or quadruple)
        // counting the move cost for individual letters. but this spawned the idea for h4, so I'm leaving it here for reference

        // if the character is in the RIGHT ROW
        // if it is above its goal position add 1 for every letter above it + the heuristic for every letter below it
        // if it is below its goal position add 1 for every letter above it in the row
        // if it is at its goal position add the heuristic of every letter below it
        if(rightRow(l.first, curr, gol)){
            if(gol_pos < pos){
                letters[l.first] += curr.substr(pos+1, curr.find(pos, '*')).size();
                while(gol_pos < pos){
                    letters[l.first] += letters[curr.at(pos)];
                    pos--;
                }
            }
            else if(gol_pos > pos){
                letters[l.first] += curr.substr(pos+1, curr.find(pos, '*')).size();
            }
            else{
                while(curr.at(pos) != '*'){
                    letters[l.first] += letters[curr.at(pos)];
                    pos--;
                }

            }
        }

        // if the character is in the WRONG ROW
        // add the number of letters on top of it
        else{
            letters[l.first] += 1 + curr.substr(pos+1, curr.find(pos, '*')).size();
        }
        h += letters[l.first];
    }
    heuristic = h;
}


void Node::h4(){
    string start = state->getStringRep();
    string end = goal->getStringRep(); 

    vector<string> start_rows = makeRows(start);
    vector<string> end_rows = makeRows(end);

    map<char, bool> letter_right_row;

    for(char c: start){
        if(c != '*'){
            letter_right_row[c] = rightRow(c, start, end);
        }
    }

    int h = 0;
    bool below_bad = false;
    int curr_index = 0;
    int curr_row = 0;

    // algorithm
    // for each row
    // for each letter
    // check if the letter is in the right place in the right row
    // start at the bottom of the row
    // add +0 for every letter in the right place and right row
    // until you reach a letter in the wrong place -- then you change below_bad to true
    // every letter after the below_good is changed gets at least +1 since it must be moved out of the row
    // every letter in the right row wrong place gets +2 since it must be moved out and moved back in
    // every letter in the wrong row gets +1 since it must be moved out of the row
    for(string row : start_rows){
        below_bad = false;
        curr_index = 0;
        for(char letter : row){

            // RIGHT ROW
            if(letter_right_row[letter]){
                
                // RIGHT SPOT
                if(curr_index < end_rows.at(curr_row).size() && end_rows.at(curr_row).at(curr_index) == letter){
                    if(below_bad){
                        // if below it is bad the letter must be moved anyway
                        h += 2;
                    }
                    else{
                        h += 0;
                    }
                }

                // WRONG SPOT
                else{
                    h += 2;
                }
            }

            // WRONG ROW
            else{
                h += 1;
                //h += (0.15) * (start_rows.at(curr_row).size() - curr_index);
                below_bad = true;
            }

            curr_index++;
        }

        curr_row++;
    }
    
    heuristic = h;
}


vector<string> Node::makeRows(string s){
    vector<string> vals = {};
    string s_curr = "";
    while(s.find('*') != -1){
        s_curr = s.substr(0, s.find('*'));
        s = s.substr(s.find('*')+1);
        vals.push_back(s_curr);
    }
    return vals;
}


bool Node::rightRow(char letter, string s1, string s2){
    // gets the string rep up until the first row divide
    string s1_r = s1.substr(0, s1.find('*'));
    string s2_r = s2.substr(0, s2.find('*'));
   
    // goes row by row until letter is found in s1
    while(s1_r.find(letter) == -1){
        s1 = s1.substr(s1.find('*')+1);
        s2 = s2.substr(s2.find('*')+1);

        s1_r = s1.substr(0, s1.find('*'));
        s2_r = s2.substr(0, s2.find('*'));
    }

    // if letter is not in the same row in s2, it is not in the right row
    if(s2_r.find(letter) == -1){
        return false;
    }

    return true;
}


int Node::numMismatch(string s1, string s2){
    int count = 0;
    for(size_t i = 0; i < s1.size(); i++){
        if(s2.size() <= i || s1.at(i) != s2.at(i)){
            count++;
        }
    }
    return count;
}

