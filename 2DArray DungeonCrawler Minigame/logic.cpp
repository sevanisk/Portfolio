#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include "logic.h"

using namespace std;

/**
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream inFS;
    inFS.open(fileName);

    // file cannot be opened
    if(!inFS.is_open()){
        return nullptr;
    }

    // first line
    string currLine = "";
    getline(inFS, currLine);
    istringstream inSS(currLine);

    inSS >> maxRow;
    if(inSS.fail()){
        return nullptr;
    }
    inSS >> maxCol;
    if(inSS.fail()){
        return nullptr;
    }

    if(maxRow <= 0 || maxCol <= 0 || INT32_MAX/maxRow < maxCol || INT32_MAX/maxCol < maxRow){
        return nullptr;
    }

    getline(inFS, currLine);
    istringstream inSS2(currLine);
    inSS2 >> player.row;
    if(inSS2.fail()){
        return nullptr;
    }
    inSS2 >> player.col;
    if(inSS2.fail()){
        return nullptr;
    }

    if(player.row < 0 || player.col < 0 || player.row >= maxRow || player.col >= maxCol){
        return nullptr;
    }

    char** level = createMap(maxRow, maxCol);

    for (int i = 0; i < maxRow; i++){
        getline(inFS, currLine);
        istringstream inSS3(currLine);
        for(int j = 0; j < maxCol; j++){
            inSS3 >> level[i][j];
            if(inSS3.fail()){
                return nullptr;
            }
        }
    }

    bool exit = false;
    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            if (level[i][j] == '?' || level[i][j] == '!'){
                exit = true;
            }
        }
    }

    if(!exit){
        return nullptr;
    }

    string temp;
    inFS >> temp;
    if(temp != ""){
        return nullptr;
    }


    level[player.row][player.col] = TILE_PLAYER;
    
    /*while(currLine == ""){
        getline(inFS, currLine);
    }

    // non numeric row or col

    // 0 or negative row or column

    // number of tiles greater than INT32_MAX

    // second line

    // non numeric row or col

    cout << "I GOT HERE " << maxRow << maxCol << endl;

    level[player.row][player.col] = TILE_PLAYER;*/

    return level;
}

/**
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if(input == MOVE_UP){
        nextRow -= 1;
    }
    else if (input == MOVE_DOWN){
        nextRow += 1;
    }
    else if (input == MOVE_LEFT){
        nextCol -= 1;
    }
    else if(input == MOVE_RIGHT){
        nextCol += 1;
    }
}

/**
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** newar = new char*[maxRow]{};
    for(int i = 0; i < maxRow; i++){
        newar[i] = new char[maxCol];
    }

    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            newar[i][j] = TILE_OPEN;
        }
    }
    return newar;
}

/**
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if(map){
        for (int row = 0; row < maxRow; row++) {
            delete[] map[row];
        }
        delete[] map; 
    }
    map = nullptr;
    maxRow = 0;
}

/**
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int newMax = maxRow;
    if(maxRow <= 0 || maxCol <= 0 || map == nullptr){
        return nullptr;
    }

    char** doubledmap = createMap(maxRow*2, maxCol*2);
    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            doubledmap[i][j] = map[i][j];
            if(map[i][j] != TILE_PLAYER){
                doubledmap[i+maxRow][j] = map[i][j];
                doubledmap[i][j+maxCol] = map[i][j];
                doubledmap[i+maxRow][j+maxCol] = map[i][j];
            }
            else {
                doubledmap[i+maxRow][j] = TILE_OPEN;
                doubledmap[i][j+maxCol] = TILE_OPEN;
                doubledmap[i+maxRow][j+maxCol] = TILE_OPEN;
            }
        }
    }

    deleteMap(map, newMax);
    maxRow *= 2;
    maxCol *= 2;
    return doubledmap;
}

/**
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    int status = 0;
    if(nextRow >= maxRow || nextRow < 0 || nextCol >= maxCol || nextCol < 0){
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    else if(map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER || (map[nextRow][nextCol] == TILE_EXIT && player.treasure < 1)){
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    else if(map[nextRow][nextCol] == TILE_TREASURE){
        player.treasure += 1;
        status = STATUS_TREASURE;
    }

    else if(map[nextRow][nextCol] == TILE_AMULET){
        status = STATUS_AMULET;
    }

    else if(map[nextRow][nextCol] == TILE_DOOR){
        status = STATUS_LEAVE;
    }

    else if(map[nextRow][nextCol] == TILE_EXIT){
        status = STATUS_ESCAPE;
    }
    else {
        status = STATUS_MOVE;
    }
    map[player.row][player.col] = TILE_OPEN;
    map[nextRow][nextCol] = TILE_PLAYER;

    player.row = nextRow;
    player.col = nextCol;

    return status;
}

/**
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    // going up 
    for(int up = player.row-1; up >= 0; up--){
        if (map[up][player.col] == TILE_PILLAR){
            break;
        }
        else if(map[up][player.col] == TILE_MONSTER){
            map[up][player.col] = TILE_OPEN;
            map[up+1][player.col] = TILE_MONSTER;
        }
    }
    for(int down = player.row+1; down < maxRow; down++){
        if (map[down][player.col] == TILE_PILLAR){
            break;
        }
        else if(map[down][player.col] == TILE_MONSTER){
            map[down][player.col] = TILE_OPEN;
            map[down-1][player.col] = TILE_MONSTER;
        }
    }
    for(int left = player.col-1; left >= 0; left--){
        if (map[player.row][left] == TILE_PILLAR){
            break;
        }
        else if(map[player.row][left] == TILE_MONSTER){
            map[player.row][left] = TILE_OPEN;
            map[player.row][left+1] = TILE_MONSTER;
        }
    }
    for(int right = player.col+1; right < maxCol; right++){
        if (map[player.row][right] == TILE_PILLAR){
            break;
        }
        else if(map[player.row][right] == TILE_MONSTER){
            map[player.row][right] = TILE_OPEN;
            map[player.row][right-1] = TILE_MONSTER;
        }
    }

    if(map[player.row][player.col] == TILE_MONSTER){
        return true;
    }
    return false;
}
