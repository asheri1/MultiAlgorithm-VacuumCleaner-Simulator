#include "Algorithm_B.h"
#include <iostream>
#include "../AlgorithmRegistration.h"


REGISTER_ALGORITHM(Algorithm_B);

// Constructor
Algorithm_B::Algorithm_B() {
}

// Destructor
Algorithm_B::~Algorithm_B() {
}


Step Algorithm_B::nextStep() {

    int battery = robot->getBatteryLevel();
    // DEAD
    if((remainedSteps == 0 || battery == 0) && !isAtDocking()) {
        robotIsDead = true;
        graphReduceLines();
        return Step::Finish;
    }
    // Check if there are walls around the current location and add to the graph if so
    addWalls();
    goToLastspot = graph.containX();
    // if the current loction value is 'x' replace it with the original val
    if(graph.getVal(getCurrLocation()) == 'x') {
        graph.setVal(getCurrLocation(), flagChar);
        goToLastspot = false;
    }

    // Calculate the paths
    int docking_dist = minDistance(0);           // arg == 0 => min Distance to Docking station
    int dirty_spot_dist = minDistance(1);        // arg == 1 => min Distance to dirty spot
    int new_spot_dist = minDistance(2);          // arg == 2 => min Distance to new spot
    int last_mapping_spot_dist = minDistance(3); // arg == 3 => min Distance to the last mapping spot 
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
    // if the house is clean OR the battery is low OR there are not enough steps to clean or check new spot - go to the Docking station
    if(isBatteryLow(battery, docking_dist) || isCharging || docking_dist == remainedSteps || ((2 * dirty_spot_dist) > remainedSteps)) {
        // put 'x' if we stop mapping and go charging and store the original value
        if(isBatteryLow(battery, docking_dist) && !graph.containX()) {
            flagChar = graph.getVal(getCurrLocation());
            graph.setVal(getCurrLocation(),'x');
            goToLastspot = true;
        }
        return returningToDocking();    
    }
    // if the robot don't need to charge and it's a dirty spot - clean
    if(dirtSensor->dirtLevel() > 0) { 
        stepsListLog.push_back('s');
        return Step::Stay;
    }
    // go to the spot where we left off
    if(goToLastspot && last_mapping_spot_dist != -1) {
        return goToLastMappingSpot();
    }
    // Check if the robot finishes mapping the house
    // finishedMapping == true => The robot knows the house full map
    if(!finishedMapping) {  
        finishedMappingTheHouse(); 
    }
    // finishedMapping == false => The robot still dont knows the house full map 
    if(finishedMapping) { 
        graph.updateLayout(); 
        return greedyStep(); 
    }
    Direction dir = moveInCircle(getCurrLocation());
    Step step = convertDirectionToStep(dir);
    if(moveInCircleStuck) {
        int new_radius_dist = searchLargerRadius();
        if(new_radius_dist != -1 && new_radius_dist <= new_spot_dist) {
            return goToNewRadius();
        }
        return greedyStep();
    }
    if(!moveInCircleStuck) {
        stepsListLog.push_back(convertStepToChar(step));
        return step;
    }
    // defult 
    return greedyStep();
}

// RadiusX is defined to be the column distance between the points
int Algorithm_B::calcRadiusX(int x) const {
    return (x - graph.getDockingLocation().getX());
}

// RadiusY is set to be the line distance between the points
int Algorithm_B::calcRadiusY(int y) const {
    return (y - graph.getDockingLocation().getY());
}

//Radius is defined to be the greater distance between radiusX and radiusY
int Algorithm_B::calcRadius(int x, int y) {
    int radX = calcRadiusX(x);
    int radY = calcRadiusY(y);
    if(radX < 0) {
        radX = -1 * radX;
    }
    if(radY < 0) {
        radY = -1 * radY;
    }
    int maxRad = radX > radY ? radX : radY;
    if(maxRad > maxRadiusSoFar) {
        maxRadiusSoFar = maxRad;
    }
    return maxRad;
}

// moves in a circle with a constant radius around the docking station and increases the radius when it completes a circle
Direction Algorithm_B::moveInCircle(Coordinates temp) {
    Direction dir;
    int x = temp.getX(); 
    int y = temp.getY();
    // radiosX = x - dockX , radiosY = y - dockY
    int radiosX = calcRadiusX(x);
    int radiosY = calcRadiusY(y);
    
    if(goToLastspot) {
        if(graph.getVal(getCurrLocation()) == 'x') {
            graph.setVal(getCurrLocation(), flagChar);
            goToLastspot = false;
            moveInCircleStuck = false;
        }
        else{
            return convertStepToDirection(goToLastMappingSpot());
        }
    }

    // current location = docking station
    if(radiosX == 0 && radiosY == 0) { 
        if(goToLastspot){
            goToLastMappingSpot();
        }
        return convertStepToDirection(greedyStep());
    } 
    // current location - I
    else if(radiosX <= 0 && radiosY <= 0) { 
        if (radiosX <= radiosY)  { 
            dir = Direction::South;  
        }
        else { // (radiosX > radiosY) 
            dir = Direction::West;  
        }
    }
    // current location - II
    else if(radiosX > 0 && radiosY <= 0) { 
        int absRadiusY = (-1 * radiosY); 
        if ((radiosX + 1) <= absRadiusY)  { 
            dir = Direction::West;  
        }
        else {  
            dir = Direction::North;  
        }
    }
    // current location - III
    else if(radiosX >= 0 && radiosY > 0) { 
        if(radiosX >= radiosY) { 
            dir = Direction::North; 
        }
        else { //radiosX < radiosY  
            dir = Direction::East; 
        }
    }
    // current location - IV
    else { //(radiosX < 0 && radiosY >= 0)  
        int absRadiusX = (-1 * radiosX);
        if(absRadiusX <= radiosY) { 
            dir = Direction::East; 
        }
        else {  
            dir = Direction::South; 
        }
    }

    bool faild = false;
    Coordinates coor = getCurrLocation();
    coor = moveCoordinates(coor,dir);
    while(wallSensor->isWall(dir) || graph.getVal(coor.getX(),coor.getY()) == 'W') { 
        if(faild) {
            moveInCircleStuck = true;
            maxRadiusSoFar = calcRadius(getCurrLocation().getX(),getCurrLocation().getY());
            Step step = greedyStep(); 
            if(searchLargerRadius() != -1) {
                step = goToNewRadius();
            }
            return convertStepToDirection(step);
        }
        switch (dir) {
            case Direction::North:
                faild = true;
                dir = Direction::East;
                break;
            case Direction::South:
                faild = true;
                dir = Direction::West;
                break;
            case Direction::East:
                faild = true;
                dir = Direction::South;
                break;
            case Direction::West:
                faild = true;
                dir = Direction::North;
                break;
        }
        coor = getCurrLocation();
        coor = moveCoordinates(coor,dir);
    }
    return dir;
}

// checks if the robot has finished mapping the house and if so returns "true"
void Algorithm_B::finishedMappingTheHouse() {
    graph.resize();
    int rows = graph.getNumOfRows();
    int cols = graph.getNumOfCols();

    // Queue for BFS and a set for visited cells
    std::queue<Coordinates> q;
    std::vector<std::vector<bool>> visited(cols, std::vector<bool>(rows, false));
      
    // Start BFS from the current location
    Coordinates current = getCurrLocation();
    q.push(current);
    visited[current.getX()][current.getY()] = true;
    
    // Directions for moving in the grid (Nort, South, West, East)
    std::vector<std::pair<int, int>> directions = { {0, -1}/*Nort*/, {0, 1}/*South*/, {1, 0}/*East*/, {-1, 0}/*West*/ };
    std::vector<Direction> directionEnums = { Direction::North, Direction::South, Direction::East, Direction::West };

    // docking station and current loction coordinates
    Coordinates dest = {0,0};
    while (!q.empty()) {
        current = q.front();
        q.pop();
        // if current is out of the graph
        if(current.getX() >= cols || current.getY() >= rows || current.getX() < 0 || current.getY() < 0) {
            continue;
        }
        // Return True if the Coordinates(0,0) is reachable
        if (current == dest) {
            graph.updateLayout(); // graphReduceLines();
            finishedMapping = false;
            return;
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
            if (graph.getVal(neighbor) != 'W' && validLocation(neighbor) && !visited[newX][newY]) {
                q.push(neighbor);
                visited[newX][newY] = true;
            }
        }
    }
    // Return True if the Coordinates(0,0) is not reachable
    graphReduceLines(); 
    finishedMapping = true; 
}


Step Algorithm_B::returningToDocking() {
    // make sure the graph doesn't get too big
    if(graph.getNumOfCols() > 20 || graph.getNumOfRows() > 20) {
        graphReduceLines();
    }
    Step step;
    // already in the docking station
    if(isCharging || isAtDocking()) { 
        graph.updateLayout();
        if(remainedSteps == 0) {
            step = Step::Finish;
            stepsListLog.push_back(convertStepToChar(step));
            return step; // Finish
        }
        else if(isCharged()) {
            isCharging = false;
            // if the circular algorithm gets stuck we will switch to the greedy algorithm
            if(!moveInCircleStuck) {
                step = convertDirectionToStep(moveInCircle(getCurrLocation()));
                stepsListLog.push_back(convertStepToChar(step));
                return step;
            }
            else if(goToLastspot){
                return goToLastMappingSpot();
            }
            else{
                return greedyStep(); // MOVE
            }
        }
        else { //(still chargging)
            isCharging = true;
            step = Step::Stay;
            stepsListLog.push_back(convertStepToChar(step));
            return step; // CHARGE
        }
    }
    else { // Move - on the way to Docking station
        return goToDocking();
    }
}


int Algorithm_B::searchLargerRadius() {

    // the radius we are looking for
    int radius = maxRadiusSoFar + 1;
    // Dimensions of the graph
    int rows = graph.getNumOfRows();
    int cols = graph.getNumOfCols();
    // Stack for DFS and a set for visited cells
    std::vector<Coordinates> s;
    std::vector<std::vector<bool>> visited(cols, std::vector<bool>(rows, false));
    std::map<Coordinates, std::pair<Coordinates, Direction>> parent; // To track the path
    // Start BFS from the current location
    Coordinates current = getCurrLocation();
    s.push_back(current);
    visited[current.getX()][current.getY()] = true;
    // Directions for moving in the grid (North, South, West, East)
    //                                               North    East    South    West
    std::vector<std::pair<int, int>> directions = { {0, -1}, {0, 1}, {1, 0}, {-1, 0} };
    std::vector<Direction> directionEnums = { Direction::North, Direction::South, Direction::East, Direction::West };
    int dis = 0;
    while (!s.empty()) {
        current = s.front();
        s.erase(s.begin());
        // if current is out of the graph
        if (current.getX() >= cols || current.getY() >= rows || current.getX() < 0 || current.getY() < 0) {
            continue;
        }
        // calc radius
        dis = calcRadius(current.getX(), current.getY());
        // check if the current cell is the radius we are looking for
        if (dis == radius) {
            break;
        }
        // check neighbors
        int newX = -1;
        int newY = -1;
        for (int i = 0; i < 4; ++i) {
            newX = current.getX() + directions[i].first;
            newY = current.getY() + directions[i].second;
            if (newX >= cols || newX < 0 || newY >= rows || newY < 0) {
                continue;
            }
            Coordinates neighbor = { newX, newY };
            if (validLocation(neighbor) && !visited[newX][newY]) {
                s.push_back(neighbor);
                visited[newX][newY] = true;
                parent[neighbor] = { current, directionEnums[i] };
            }
        }
    }
    /*************************************************/
    // Return -1 if the destination is not reachable  */
    /*************************************************/
    if (dis != radius) {
        moveInCircleStuck = true;
        return -1;
    }
    /*********************************/
    // creating path to the docking  */
    /*********************************/
    while (!(current == getCurrLocation())) {
        auto p = parent[current];
        pathToNewRadius.push_back(p.second);
        current = p.first;
    }
    // The path is in reverse order, so reverse it
    std::reverse(pathToNewRadius.begin(), pathToNewRadius.end());
    return dis;
}


Step Algorithm_B::goToLastMappingSpot() {
    Direction dir = pathToLastMappingSpot.front();
    pathToLastMappingSpot.erase(pathToLastMappingSpot.begin());
    Step step = convertDirectionToStep(dir); // Move
    stepsListLog.push_back(convertStepToChar(step));
    return step;
}


Step Algorithm_B::goToNewRadius() {
    Direction dir = pathToNewRadius.front();
    pathToNewRadius.erase(pathToNewRadius.begin());
    Step step = convertDirectionToStep(dir); // Move
    stepsListLog.push_back(convertStepToChar(step));
    return step;
}

// BFS function for minimum steps until next spot and initialize the relevant path 
// dest = 0 (for docking)
// dest = 1 (for dirty spot) 
// dest = 2 (for new spot)
// dest = 3 (for the last mapping spot)
int Algorithm_B::minDistance(int dest) {

    bool goToDocking = false;
    bool goToClean = false;
    bool goToNewSpot = false;
    bool goMapping = false;
    Coordinates current = getCurrLocation();

    // Clear the relevant queue and initialize it variable 
    if(dest == 0) { goToDocking = true; pathToDocking.clear();}
    if(dest == 1) { goToClean = true; pathToDirtySpot.clear();}
    if(dest == 2) { goToNewSpot = true; pathToNewSpot.clear();}
    if(dest == 3) { goMapping = true; pathToLastMappingSpot.clear();}

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
    // case 3 - found the last mapping Spot
    else if(goMapping && graph.getVal(current) == 'x') {
        return 0;
    }
    // defult case - the argument is not recognized
    else {
        if(!goToDocking && !goToClean && !goToNewSpot && !goMapping) {
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
        // case 3 - check if the current cell is 'x'  
        if(goMapping && graph.getVal(current) == 'x') {
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
            if(goToDocking || goMapping){
                if (graphVal != '-' && validLocation(neighbor) && !visited[newX][newY]) {
                    q.push(neighbor);
                    visited[newX][newY] = true;
                    distance[newX][newY] = distance[current.getX()][current.getY()] + 1;
                    parent[neighbor] = {current, directionEnums[i]};
                }
            }
            else { // (goToNewSpot || goToClean)
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
    if(goMapping && graph.getVal(current) != 'x') { return -1; }
    
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
    // Creating the route to the spot were we stoped
    if(dest == 3) { 
        while (!(current == getCurrLocation())) {
            auto p = parent[current];
            pathToLastMappingSpot.push_back(p.second);
            current = p.first;
        }
        // The path is in reverse order, so reverse it
        std::reverse(pathToLastMappingSpot.begin(), pathToLastMappingSpot.end());
    }
    return dis;
}


bool Algorithm_B::validLocation(Coordinates coor) const {
    char c = graph.getVal(coor);
    int rowSize = graph.getNumOfRows();
    int colSize = graph.getNumOfCols();
    int x = coor.getX();
    int y = coor.getY();
    
    bool b1 = ((c >= '0' && c <= '9') || c == ' ' || c == 'D' || c == '-'|| c == 'x');
    bool b2 = (x >= 0 && y >= 0 && x <= colSize && y <= rowSize);
    return b1 && b2;
}
