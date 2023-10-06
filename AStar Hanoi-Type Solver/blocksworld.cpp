 
#include<iostream>
#include<vector>
#include<cstring>
#include <fstream>
#include<map>
#include <queue>
#include <cstdlib>
#include <unistd.h>



#include "state.h"
#include "parser.h"
#include "node.h"

using namespace std;
int iter = 0;
int max_iter = 100000;
int maxq = 0;

// checks if two Nodes are equal using the hashkey
// used to check if the goal state has been reached
bool equals(Node* a, Node* b){
    if(a->getHashKey().compare(b->getHashKey()) == 0){
        return true;
    }
    return false;
}


// ASTAR!!!
// pretty much exactly what the slides said for astar 
Node* AStar(Node* start, Node* goal, int h_meth){

    Node* curr = nullptr;

    // if the start and goal are equal, RETURN START
    if (equals(start, goal)){
        return start;
    }

    // creates the priority queue and adds the start node
    priority_queue<Node, vector<Node*>, PQ> frontier;
    frontier.push(start);

    // creates a dictionary to store reached nodes, with the string as the key and adds the start node
    map<string, Node*> reached;
    reached[start->getHashKey()]=start;

    // loop while there are still children -- if there are no children left, Astar FAILED
    while(!frontier.empty()){
        
        // checks if over max iterations
        iter++;
        if(iter%1000 == 0){
            cout << "ITERATIONS SO FAR: " << iter << endl;
        }
        if(iter > max_iter){
            return curr;
        }

        // get the top of the pqueue for the frontier
        curr = frontier.top();
        frontier.pop();

        // calculate children to store in curr node
        // children calculated AS NEEDED, not every time a node is created
        curr->findChildren(reached, goal->getState(), h_meth);
        
        // for each child, 
        // if it is the goal, return it
        // otherwise add to the frontier and reached
        for(size_t index = 0; index < curr->getChildren().size(); index++){
            if(equals(curr->getChildren().at(index), goal)){
                return curr->getChildren().at(index);
            }

            reached[curr->getChildren().at(index)->getHashKey()] = curr->getChildren().at(index);
            frontier.push(curr->getChildren().at(index));

            //updates maxq
            if(frontier.size() > maxq){
                maxq = frontier.size();
            }
        }
    }

    // fail
    cout << "FAILURE" << endl;
    return curr;
}


// BFS
// made before ASTAR, but mostly similar in how it works
Node* BFS(Node* start, Node* goal, int h){
    Node* curr = nullptr;
    if (equals(start, goal)){
        return start;
    }

    queue<Node*> frontier ;
    frontier.emplace(start);

    map<string, Node*> reached;
    reached[start->getHashKey()]=start;

    while(!frontier.empty()){
        iter++;
        if(iter > max_iter){
            return curr;
        }

        curr = frontier.front();
        frontier.pop();

        curr->findChildren(reached, goal->getState(), h);
        

        for(size_t index = 0; index < curr->getChildren().size(); index++){
            if(equals(curr->getChildren().at(index), goal)){
                return curr->getChildren().at(index);
            }

            reached[curr->getChildren().at(index)->getHashKey()] = curr->getChildren().at(index);
            frontier.emplace(curr->getChildren().at(index));

            //updates maxq
            if(frontier.size() > maxq){
                maxq = frontier.size();
            }
        }
    }

    cout << "FAILURE2" << endl;
    return curr;
}

// retraces the path and prints out statistics and the path
void retracePathForwards(Node* start, Node* curr, string fname){
    
    // creates a vector of Nodes and works backward from the goal to add the path to the vector
    vector<Node*> path = {};
    while(!equals(curr, start)){
        path.insert(path.begin(), curr);
        curr = curr->getParent();
    }

    // adds start node
    path.insert(path.begin(), start);

    // prints the path in the proper order
    int depth = 0;
    for(Node* n : path){
        cout << endl << "DEPTH: " << depth;
        cout << endl << "HEURISTIC VALUE: " << n->getHeuristic();
        cout << endl << "STATE: " << endl;
        n->printNode();
        cout << endl  << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        depth++;
    }

    depth --;
    cout << "TOTAL ITERATIONS: " << iter << endl;
    cout << endl;
    cout << "STATISTICS: " << fname << " planlen " << depth << " iter " << iter << " maxq " << maxq << endl;
}

// backwards
void retracePath(Node* start, Node* curr){
    cout << "PATH: " << endl;
    int depth = 0;
    while(!equals(curr, start)){
        cout << endl << endl << endl << "DEPTH: " << depth << endl;
        curr->printNode();
        curr = curr->getParent();
        cout << endl;
        depth++;
    }
}

int main(int argc, char *argv[]){
    int opt;
    int h_method = 3;
    string filename;
    iter = 0;
    maxq = 0;

    while ((opt = getopt(argc, argv, "f:H:")) != -1) {
		switch (opt) {
			case 'f':
				filename += optarg;
				break;
			case 'H':
				h_method = atoi(optarg);
				break;
		}
	}

    Parser* p = new Parser(filename);

    Node* root = new Node(p->getStart(), p->getGoal(), h_method);
    Node* goal = new Node(p->getGoal(), p->getGoal(), h_method);
    Node* out = root;
    
    switch(h_method){
        case 0:
            out = BFS(root, goal, 1);
            break;
        case 1:
            out = AStar(root, goal, 1);
            break;

        case 2:
            out = AStar(root, goal, 2);
            break;

        case 3:
            out = AStar(root, goal, 3);
            break;
    }

    if(!equals(out, goal)){
        cout << "STATISTICS: " << filename << " FAILED" << endl;
        cout << endl << endl;
    }
    else{
        retracePathForwards(root, out, filename);
    }

    return 0;
}


/* TESTING

// TESTS FOR CLASS AND MAIN FUNCTIONALITY

// PARSER -- PARSING START AND GOAL FROM A FILE
Parser* p = new Parser(filename);
cout << p->getBlocks() << endl;
cout << p->getStacks() << endl;



// STATE -- CONSTRUCTOR IN PARSER AND STRING REPRESENTATION
// check that blocks have been added to state correctly
cout << "STRINGREP: " << p->getStart()->getStringRep() << endl;
p->getStart()->printState();
p->getGoal()->printState();



// STATE -- MOVEBLOCK
p->getStart()->moveBlock(0, 2);



// NODE CLASS -- CONSTRUCTOR, PRINT, FINDCHILDREN
vector<Node*> root_children = root->findChildren();
    for(Node* n : root_children){
        n->printNode();
        //n->getParent()->printNode();
    }



// EQUALS HELPER FUNCTION -- SHOULD RETURN 1
cout << "EQUALS" << endl;
cout << equals(root, root);

// ASTAR TRACING
cout << endl << endl << "CURRENT" << endl;
curr->printNode();
Sleep(6000);

// SLOW ITERATION TRACING THROUGH BFS
cout << "CURRENT" << endl;
curr->printNode();

cout << "PARENT" << endl;
if(curr->getParent() != nullptr){
    curr->getParent()->printNode();
}
cout << endl;

Sleep(5000);

// MAKE ROWS NODE METHOD FUNCTIONALITY
vector<string> test = root->makeRows(root->getState()->getStringRep());
for(string s: test){
    cout << s << endl;
}



// CONNECTING TO COMPUTE
// ON TAMU WIFI
// ssh sevanisk@compute.cs.tamu.edu
// enter tamu password
// add stuff to z drive

// make help
// g++ -o blocksworld *.cpp
//-m $(heuristic) -i$(file) -t $(args)

*/


