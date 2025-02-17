#include "Algorithm_A.h"
#include <iostream>
#include "../AlgorithmRegistration.h"


REGISTER_ALGORITHM(Algorithm_A);

// Constructor
Algorithm_A::Algorithm_A() : MyAlgorithm() {
}


// Destructor
Algorithm_A::~Algorithm_A() {
}



Step Algorithm_A::nextStep() {

    int battery = robot->getBatteryLevel();
    // DEAD
    if((remainedSteps == 0 || battery == 0) && !isAtDocking()) {
        robotIsDead = true;
        graphReduceLines();
        return Step::Finish;
    }
    // Check if there are walls around the current location and add to the graph if so
    addWalls();
    // Calculate the paths
    int docking_dist = minDistance(0);           // arg == 0 => min Distance to Docking station
    int dirty_spot_dist = minDistance(1);        // arg == 1 => min Distance to dirty spot
    int new_spot_dist = minDistance(2);          // arg == 2 => min Distance to new spot
    // FINISH
    int batteryDiff = robot->numberOfStepsToFullCharge();
    if(isAtDocking()) {
        if((remainedSteps == 1 || remainedSteps == 0) || (!graph.containX() && new_spot_dist == -1 && dirty_spot_dist == -1) 
            || (isCharging && (((2 * dirty_spot_dist + 1 + batteryDiff) > remainedSteps) || ((2 * new_spot_dist + 1 + batteryDiff) > remainedSteps)))) {
            stepsListLog.push_back('F');
            graphReduceLines();
            return Step::Finish;
        }
    }
    if(battery == robot->getMaxBattery()) {
        isCharging = false;
    }
    // if the house is clean OR the battery is low OR there are not enough steps to clean or check new spot - go to the Docking station
    if(isBatteryLow(battery, docking_dist) || isCharging || docking_dist == remainedSteps) {
        return returningToDocking();    
    }
    bool b1 = ((dirty_spot_dist != -1) && (docking_dist + dirty_spot_dist) >= remainedSteps);
    bool b2 = ((new_spot_dist != -1) && (docking_dist + new_spot_dist) >= remainedSteps);
    if(b1 && b2) {
        return returningToDocking();    
    }
    // if this spot is dirty stay and clean
    if(dirtSensor->dirtLevel() > 0) { 
        Step step = Step::Stay;
        stepsListLog.push_back(convertStepToChar(step));
        return step;
    }
    // resize the graph and try to find again
    if(dirty_spot_dist == -1 && new_spot_dist == -1) {
        graph.resize();
        new_spot_dist = minDistance(2);
        if(new_spot_dist == -1 && isAtDocking()) {
            stepsListLog.push_back(convertStepToChar(Step::Stay));
            return Step::Finish;
        }
        return goToDocking();
    }
    // (dirty_spot_dist != -1 || new_spot_dist != -1) 
    return greedyStep();
}



int Algorithm_A::minDistance(int dest) {

    bool goToDocking = false;
    bool goToClean = false;
    bool goToNewSpot = false;
    Coordinates current = getCurrLocation();

    // Clear the relevant queue and initialize it variable 
    if(dest == 0) { goToDocking = true; pathToDocking.clear();}
    if(dest == 1) { goToClean = true; pathToDirtySpot.clear();}
    if(dest == 2) { goToNewSpot = true; pathToNewSpot.clear();}
    
    // case 0 - already at a docking station    
    if(goToDocking && isAtDocking()) {
        return 0;
    }
    // case 1 - found dirty spot  
    else if(goToClean && graph.getVal(current) > '0' && graph.getVal(current) <= '9') {
        return 0;
    }
    // case 2 - found new spot  
    else if(goToNewSpot && graph.getVal(current) == '-') {
        return 0;
    }
    // defult case - the argument is not recognized
    else {
        if(!goToDocking && !goToClean && !goToNewSpot) {
            return -1;
        }
    }

    // Dimensions of the graph
    int rows = graph.getNumOfRows();
    int cols = graph.getNumOfCols();

    // Queue for BFS and a set for visited cells
    std::queue<Coordinates> q;
    std::vector<std::vector<bool>> visited(cols, std::vector<bool>(rows, false));
    std::vector<std::vector<int>> distance(cols, std::vector<int>(rows, std::numeric_limits<int>::max()));
    std::map<Coordinates, std::pair<Coordinates, Direction>> parent; // To track the path
      
    // Start BFS from the current location
    q.push(current);
    visited[current.getX()][current.getY()] = true;
    distance[current.getX()][current.getY()] = 0;
    
    // Directions for moving in the grid (Nort, South, West, East)
    //                                               North   South    East    West
    std::vector<std::pair<int, int>> directions = { {0, -1}, {0, 1}, {1, 0}, {-1, 0} };
    std::vector<Direction> directionEnums = { Direction::North, Direction::South, Direction::East, Direction::West };

    // docking station and current loction coordinates
    int dis = 0;
    Coordinates dock = graph.getDockingLocation();
    //char graphVal;
    while (!q.empty()) {
        current = q.front();
        q.pop();
        // if current is out of the graph
        if(current.getX() >= cols || current.getY() >= rows || current.getX() < 0 || current.getY() < 0) {
            continue;
        }
        // case 0 - check if the current cell is the docking station
        if (goToDocking && current == dock) {
            dis = distance[current.getX()][current.getY()];
            break;
        }
        // case 1 - check if the current cell is a dirty spot  
        if(goToClean && graph.getVal(current) > '0' && graph.getVal(current) <= '9') {
            dis = distance[current.getX()][current.getY()];
            break;
        }
        // case 2 - check if the current cell is a new spot  
        if(goToNewSpot && graph.getVal(current) == '-') {
            dis = distance[current.getX()][current.getY()];
            break;
        }
        
        // check neighbors
        int newX = -1;
        int newY = -1;
        for (int i = 0; i < 4; ++i) {
            newX = current.getX() + directions[i].first;
            newY = current.getY() + directions[i].second;
            if (newX >= cols || newX < 0 || newY >= rows || newY < 0) {
                continue;;
            }
            Coordinates neighbor = {newX, newY};
            char graphVal = graph.getVal(neighbor);
            if(goToDocking || goToClean){
                if (graphVal != '-' && validLocation(neighbor) && !visited[newX][newY]) {
                    q.push(neighbor);
                    visited[newX][newY] = true;
                    distance[newX][newY] = distance[current.getX()][current.getY()] + 1;
                    parent[neighbor] = {current, directionEnums[i]};
                }
            }
            else { // (goToNewSpot || goMapping)
                if (validLocation(neighbor) && !visited[newX][newY]) {
                    q.push(neighbor);
                    visited[newX][newY] = true;
                    distance[newX][newY] = distance[current.getX()][current.getY()] + 1;
                    parent[neighbor] = {current, directionEnums[i]};
                }
            }
        }
    }
    /*************************************************/
    // Return -1 if the destintion is not reachable  */
    /*************************************************/
    if(goToDocking && !(current == dock)) {  return -1; }
    if(goToClean && (graph.getVal(current) < '0' || graph.getVal(current) > '9')) { return -1; }
    if(goToNewSpot && graph.getVal(current) != '-') { return -1; }
    /*************************************/
    // creating path to the destination  */
    /*************************************/
    // creating path to the Docking Station
    if(dest == 0) { 
        current = graph.getDockingLocation();
        while (!(current == getCurrLocation())) {
            auto p = parent[current];
            pathToDocking.push_back(p.second);
            current = p.first;
        }
        // The path is in reverse order, so reverse it
        std::reverse(pathToDocking.begin(), pathToDocking.end());
    }
    // Creating the route to the dirty spot
    if(dest == 1) { 
        while (!(current == getCurrLocation())) {
            auto p = parent[current];
            pathToDirtySpot.push_back(p.second);
            current = p.first;
        }
        // The path is in reverse order, so reverse it
        std::reverse(pathToDirtySpot.begin(), pathToDirtySpot.end());
    }
    // Creating the route to new spot
    if(dest == 2) { 
        while (!(current == getCurrLocation())) {
            auto p = parent[current];
            pathToNewSpot.push_back(p.second);
            current = p.first;
        }
        // The path is in reverse order, so reverse it
        std::reverse(pathToNewSpot.begin(), pathToNewSpot.end());
    }
    return dis;
}



bool Algorithm_A::validLocation(Coordinates coor) const {
    char c = graph.getVal(coor);
    int rowSize = graph.getNumOfRows();
    int colSize = graph.getNumOfCols();
    int x = coor.getX();
    int y = coor.getY();
    
    bool b1 = ((c >= '0' && c <= '9') || c == ' ' || c == 'D' || c == '-');
    bool b2 = (x >= 0 && y >= 0 && x <= colSize && y <= rowSize);
    return b1 && b2;
}