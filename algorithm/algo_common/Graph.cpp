#include "Graph.h"
#include <iostream>
#include <algorithm>



Graph::Graph() : docking(5, 5), currLocation(5, 5) {
    layout.resize(11, std::vector<char>(11, '-'));
    layout[docking.getY()][docking.getX()] = 'D';
}

// adds a node to the graph
void Graph::addNode(const Coordinates& coords, char dirtLevel) {
    int x = coords.getX();
    int y = coords.getY();

    // It is not possible to add another docking station
    if(dirtLevel == 'D') {
        dirtLevel = ' ';
    }

    if(layout[y][x] > '0' && layout[y][x] <= '9') {
        return;
    }

    // Do not change the docking station
    if (x == docking.getX() && y == docking.getY()) {
        return;
    }

    if (x < 0 || x >= static_cast<int>(layout.size()) || y < 0 || y >= static_cast<int>(layout[0].size())) {
        resize();
        x = coords.getX() + 5;
        y = coords.getY() + 5;
    }
    layout[y][x] = dirtLevel;
}

// cleans and thus reduces the level of dirt
void Graph::clean() {
    char dirtLevel = layout[currLocation.getY()][currLocation.getX()];
    if(dirtLevel > '0' && dirtLevel <= '9'){
        layout[currLocation.getY()][currLocation.getX()]--;
    }
    if(layout[currLocation.getY()][currLocation.getX()] == '0') {
        layout[currLocation.getY()][currLocation.getX()] = ' ';
    }
}

// expands the graph in all directions
void Graph::resize() {
    int numOfRows = getNumOfRows() + 2;
    int numOfCols = getNumOfCols() + 2;
    // Create new layout with larger size
    std::vector<std::vector<char>> newLayout(numOfRows, std::vector<char>(numOfCols, '-'));
    
    // Copy old layout to the center of the new layout
    int offset = 1;
    for (int i = 0; i < static_cast<int>(layout.size()); ++i) {
        for (int j = 0; j < static_cast<int>(layout[i].size()); ++j) {
            newLayout[i + offset][j + offset] = layout[i][j];
        }
    }
    // Update layout
    layout = newLayout;
    // Update docking and currLocation coordinates
    docking.setX(docking.getX() + offset);
    docking.setY(docking.getY() + offset);
    currLocation.setX(currLocation.getX() + offset);
    currLocation.setY(currLocation.getY() + offset);
}

// extends the graph only towards the North
void Graph::resizeN() {
    // Create new layout with an additional row
    int numOfRows = getNumOfRows() + 1;
    int numOfCols = getNumOfCols();
    std::vector<std::vector<char>> newLayout(numOfRows, std::vector<char>(numOfCols, '-'));

    // Copy old layout to the lower part of the new layout
    for (int i = 0; i < static_cast<int>(layout.size()); ++i) {
        for (int j = 0; j < static_cast<int>(layout[i].size()); ++j) {
            newLayout[i + 1][j] = layout[i][j];
        }
    }

    // Update layout
    layout = newLayout;

    // Update docking and currLocation coordinates
    docking.setY(docking.getY() + 1);
    currLocation.setY(currLocation.getY() + 1);
}

// expands the graph only towards the South
void Graph::resizeS() {
    // Create new layout with an additional column
    int numOfRows = getNumOfRows() + 1;
    int numOfCols = getNumOfCols();
    std::vector<std::vector<char>> newLayout(numOfRows, std::vector<char>(numOfCols, '-'));

    // Copy old layout to the new layout without changing its position
    for (int i = 0; i < static_cast<int>(layout.size()); ++i) {
        for (int j = 0; j < static_cast<int>(layout[i].size()); ++j) {
            newLayout[i][j] = layout[i][j];
        }
    }

    // Update layout
    layout = newLayout;
}

// extends the graph only towards the West
void Graph::resizeW() {
    // Create new layout with an additional column
    int numOfRows = getNumOfRows();
    int numOfCols = getNumOfCols() + 1;
    std::vector<std::vector<char>> newLayout(numOfRows, std::vector<char>(numOfCols, '-'));

    // Copy old layout to the new layout without changing its position
    for (int i = 0; i < static_cast<int>(layout.size()); ++i) {
        for (int j = 0; j < static_cast<int>(layout[i].size()); ++j) {
            newLayout[i][j+1] = layout[i][j];
        }
    }

    // Update layout
    layout = newLayout;

    // Update docking and currLocation coordinates
    docking.setX(docking.getX() + 1);
    currLocation.setX(currLocation.getX() + 1);
}

// expands the graph only towards the East
void Graph::resizeE() {
    // Create new layout with an additional column
    int numOfRows = getNumOfRows();
    int numOfCols = getNumOfCols() + 1;
    std::vector<std::vector<char>> newLayout(numOfRows, std::vector<char>(numOfCols, '-'));

    // Copy old layout to the new layout without changing its position
    for (int i = 0; i < static_cast<int>(layout.size()); ++i) {
        for (int j = 0; j < static_cast<int>(layout[i].size()); ++j) {
            newLayout[i][j] = layout[i][j];
        }
    }

    // Update layout
    layout = newLayout;
}

// removes unnecessary lines in the graph that contain only '-'
bool Graph::updateLayout(){
    bool b1 = updateNorthLayout();
    bool b2 = updateSouthLayout();
    bool b3 = updateWestLayout();
    bool b4 = updateEastLayout();
    return b1 || b2 || b3 || b4;
}


// checks if the top two rows contain only '-' if so deletes the top row from the graph
bool Graph::updateNorthLayout() {
    // Check if there are at least two rows
    if (layout.size() < 2) {
        return false;
    }
    // Check if the first two rows contain only '-'
    bool firstRowOnlyDashes = true;
    bool secondRowOnlyDashes = true;

    for (char c : layout[0]) {
        if (c != '-') {
            firstRowOnlyDashes = false;
            break;
        }
    }
    for (char c : layout[1]) {
        if (c != '-') {
            secondRowOnlyDashes = false;
            break;
        }
    }
    // If both rows contain only '-', erase the first row
    if (firstRowOnlyDashes && secondRowOnlyDashes) {
        // Update docking and currLocation coordinates
        docking.setY(docking.getY()-1);
        currLocation.setY(currLocation.getY()-1);
        layout.erase(layout.begin());
        return true;
    }
    return false;
}

// checks if the bottom two rows contain only '-' if so deletes the bottom row from the graph
bool Graph::updateSouthLayout() {
    // Check if there are at least two rows
    if (layout.size() < 2) {
        return false;
    }
    // Check if the last two rows contain only '-'
    bool lastRowOnlyDashes = true;
    bool secondLastRowOnlyDashes = true;
    for (char c : layout[layout.size() - 1]) {
        if (c != '-') {
            lastRowOnlyDashes = false;
            break;
        }
    }
    for (char c : layout[layout.size() - 2]) {
        if (c != '-') {
            secondLastRowOnlyDashes = false;
            break;
        }
    }
    // If both rows contain only '-', erase the last row
    if (lastRowOnlyDashes && secondLastRowOnlyDashes) {
        layout.pop_back();
        return true;
    }
    return false;
}

// checks if the right two cols contain only '-' if so deletes the right col from the graph
bool Graph::updateWestLayout() {
    // Check if there are at least two columns
    if (layout.empty() || layout[0].size() < 2) {
        return false;
    }

    // Check if the first two columns contain only '-'
    bool firstColOnlyDashes = true;
    bool secondColOnlyDashes = true;
    for (const auto& row : layout) {
        if (row[0] != '-') {
            firstColOnlyDashes = false;
            break;
        }
    }
    for (const auto& row : layout) {
        if (row[1] != '-') {
            secondColOnlyDashes = false;
            break;
        }
    }
    // If both columns contain only '-', erase the first column
    if (firstColOnlyDashes && secondColOnlyDashes) {
        // Update docking and currLocation coordinates
        docking.setX(docking.getX()-1);
        currLocation.setX(currLocation.getX()-1);
        for (auto& row : layout) {
            row.erase(row.begin());
        }
        return true;
    }
    return false;
}

// checks if the left two cols contain only '-' if so deletes the left col from the graph
bool Graph::updateEastLayout() {
    // Check if there are at least two columns
    if (layout.empty() || layout[0].size() < 2) {
        return false;
    }

    // Get the number of columns
    size_t numCols = layout[0].size();

    // Check if the last two columns contain only '-'
    bool lastColOnlyDashes = true;
    bool secondLastColOnlyDashes = true;
    for (const auto& row : layout) {
        if (row[numCols - 1] != '-') {
            lastColOnlyDashes = false;
            break;
        }
    }
    for (const auto& row : layout) {
        if (row[numCols - 2] != '-') {
            secondLastColOnlyDashes = false;
            break;
        }
    }
    // If both columns contain only '-', erase the last column
    if (lastColOnlyDashes && secondLastColOnlyDashes) {
        for (auto& row : layout) {
            row.pop_back();
        }
        return true;
    }
    return false;
}

// returns true if the char 'x' exists in the graph and false otherwise
bool Graph::containX() {
    for (const auto& row : layout) {
        for (const auto& cell : row) {
            if (cell == 'x') {
                return true;
            }
        }
    }
    return false;
}

// returns the location of the docking station
Coordinates Graph::getDockingLocation() const {
    return docking;
}

// returns the location of the robot in the graph
Coordinates Graph::getCurrLocation() const {
    return currLocation;
}

// returns what is in the coordinates position in the graph as a char
char Graph::getVal(Coordinates coor) const {
    return layout[coor.getY()][coor.getX()];
}

// returns what is in the coordinates position in the graph as a char
char Graph::getVal(int x, int y) const {
    return layout[y][x];
}


// Performs placement in the coordinates position in the graph to the char in the argument
void Graph::setVal(Coordinates coor, char c) {
    layout[coor.getY()][coor.getX()] = c;
}


int Graph::getNumOfRows() const {
    return static_cast<int>(layout.size());
}


int Graph::getNumOfCols() const {
    return static_cast<int>(layout[0].size());
}

// moves the robot on the graph according to the direction in the argument
void Graph::move(Direction d) { 
    
    switch (d) {
        case Direction::North:
            currLocation = currLocation.getCoordinatesN();
            break;
        case Direction::South:
            currLocation = currLocation.getCoordinatesS();
            break;
        case Direction::West:
            currLocation = currLocation.getCoordinatesW();
            break;
        case Direction::East:
            currLocation = currLocation.getCoordinatesE();
            break;
    }
    int x = currLocation.getX();
    int y = currLocation.getY();
    if (x < 0 || x >= static_cast<int>(layout.size()) || y < 0 || y >= static_cast<int>(layout[0].size())) {
        resize();
    }
}



/****** for Debugging *******/

// Printing the status of the graph
void Graph::printGraphStatus() const {
    std::cout << "docking station: layout[" << docking.getX() << "][" << docking.getY() << "]\n";
    std::cout << "currLocation: layout[" << currLocation.getX() << "][" << currLocation.getY() << "] = " << getVal(currLocation) << "\n";
    for (const auto &row : layout) {
        for (const auto &cell : row) {
            std::cout << cell << " ";
        }
        std::cout << '\n';
    }
    std::cout << "\n" << std::endl;
}

// Printing the status of the graph and the robot - the location of the robot is marked by '@'
void Graph::printGraph() const {
    std::cout << "docking station: layout[" << docking.getX() << "][" << docking.getY() << "]\n";
    std::cout << "   currLocation: layout[" << currLocation.getX() << "][" << currLocation.getY() << "] = " << getVal(currLocation) << "\n";
    for (std::vector<std::vector<char>>::size_type i = 0; i < layout.size(); ++i) {
        for (std::vector<char>::size_type j = 0; j < layout[i].size(); ++j) {
            if (static_cast<int>(i) == currLocation.getY() && static_cast<int>(j) == currLocation.getX()) {
                std::cout << '@' << " ";
            } else {
                std::cout << layout[i][j] << " ";
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n" << std::endl;
}

