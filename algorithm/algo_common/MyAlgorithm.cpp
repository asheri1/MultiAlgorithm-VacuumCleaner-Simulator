#include "MyAlgorithm.h"
#include <iostream>


MyAlgorithm::~MyAlgorithm() {}


MyAlgorithm::MyAlgorithm() 
    : graph(), robot(nullptr), wallSensor(nullptr), dirtSensor(nullptr), 
    batteryMeter(nullptr), remainedSteps(0), maxSteps(0) 
{}


void MyAlgorithm::initAlgo(VacuumCleaner& robot, WallsSensor& wallSensor, DirtSensor& dirtSensor, BatteryMeter& batteryMeter){
    
    this->robot = &robot; 
    setWallsSensor(wallSensor); 
    setDirtSensor(dirtSensor);
    setBatteryMeter(batteryMeter); 
    setMaxSteps(remainedSteps);
}


void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {
    this->maxSteps = maxSteps;
}


void MyAlgorithm::setWallsSensor(const WallsSensor& sensor) {
    this->wallSensor = &sensor;
}


void MyAlgorithm::setDirtSensor(const DirtSensor& sensor) {
    this->dirtSensor = &sensor;
}


void MyAlgorithm::setBatteryMeter(const BatteryMeter& meter) {
    this->batteryMeter = &meter;
}


void MyAlgorithm::setRemainedSteps(int steps){
    remainedSteps = steps;
}


Step MyAlgorithm::greedyStep() {
    
    int dirty_spot_dist = minDistance(1);
    int new_spot_dist = minDistance(2);

    // no new or dirty spot found
    if(dirty_spot_dist == -1 && new_spot_dist == -1) {
        // try to resize the graph and find a new reachable spot
        graph.resize();
        new_spot_dist = minDistance(2);
        // if not found any new spot, return to the docking station
        if(new_spot_dist == -1) {
            if(isAtDocking()) {
                stepsListLog.push_back('F');
                graphReduceLines();
                return Step::Finish;
            }
            else {
                return returningToDocking();
            }
        }
    }
    // if one of the distances is -1, make it larger so that in the next test the priority will be the second number
    if(dirty_spot_dist == -1) { dirty_spot_dist = 100000; }
    if(new_spot_dist == -1)   { new_spot_dist = 100000; }
    // if the distance to a dirty spot is less or equal to the distance from a new spot go to it 
    if(dirty_spot_dist <= new_spot_dist) {
        return goToDirtySpot();
    }
    // otherwise go to a new spot - if(dirty_spot_dist > new_spot_dist)
    return goToNewSpot();
}


// deletes unnecessary lines of '-'
void MyAlgorithm::graphReduceLines() {
    while(graph.updateLayout());
}


void MyAlgorithm::addWalls() {
    Coordinates wallCoordinates = getCurrLocation();
    int colSize = graph.getNumOfCols();
    int rowSize = graph.getNumOfRows();
    int x = wallCoordinates.getX();
    int y = wallCoordinates.getY(); 
    if (x == 0 || x == colSize-1 || y == 0 || y == rowSize-1) {
        //graph.resize();
        if (y == 0) {
            graph.resizeN();
        }
        if (y == rowSize-1) {
            graph.resizeS();
        }
        if (x == 0) {
            graph.resizeW();
        }
        if (x == colSize-1) {
            graph.resizeE();
        }
    }
    Direction directions[4] = { Direction::North, Direction::East, Direction::South, Direction::West };
    for(int i = 0; i < 4; i++) {
        Direction dir = directions[i];
        wallCoordinates = getCurrLocation();
        wallCoordinates = moveCoordinates(wallCoordinates,dir);
        if(wallSensor->isWall(dir)) {
            graph.setVal(wallCoordinates, 'W');
        }
    }

}


Step MyAlgorithm::returningToDocking() {
    // make sure the graph doesn't get too big
    if(graph.getNumOfCols() > 20 || graph.getNumOfRows() > 20) {
        graphReduceLines();
    }
    Step step;
    // already in the docking station
    if(isCharging || isAtDocking()) { 
        if(remainedSteps == 0) {
            step = Step::Finish;
            stepsListLog.push_back(convertStepToChar(step));
            return step; // Finish
        }
        else if(isCharged()) {
            isCharging = false;
            return greedyStep(); // MOVE
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


Step MyAlgorithm::goToDocking() {
    Direction dir = pathToDocking.front();
    pathToDocking.erase(pathToDocking.begin());
    Step step = convertDirectionToStep(dir); // Move
    stepsListLog.push_back(convertStepToChar(step));
    return step;
}


Step MyAlgorithm::goToDirtySpot() {
    Direction dir = pathToDirtySpot.front();
    pathToDirtySpot.erase(pathToDirtySpot.begin());
    Step step = convertDirectionToStep(dir); // Move
    stepsListLog.push_back(convertStepToChar(step));
    return step;
}


Step MyAlgorithm::goToNewSpot() {
    Direction dir = pathToNewSpot.front();
    pathToNewSpot.erase(pathToNewSpot.begin());
    Step step = convertDirectionToStep(dir); // Move
    stepsListLog.push_back(convertStepToChar(step));
    return step;
}


void MyAlgorithm::move(Step step){
    robot->move(convertStepToDirection(step));
    graph.move(convertStepToDirection(step));
    if(isAtDocking()) { return; }
    char c = '0' + dirtSensor->dirtLevel();
    if(c == '0') { c = ' '; }
    graph.setVal(getCurrLocation(), c);
}


Coordinates MyAlgorithm::moveCoordinates (Coordinates coor, Direction d) {
    switch (d) {
        case Direction::North:
            coor = coor.getCoordinatesN();
            break;
        case Direction::South:
            coor = coor.getCoordinatesS();
            break;
        case Direction::West:
            coor = coor.getCoordinatesW();
            break;
        case Direction::East:
            coor = coor.getCoordinatesE();
            break;
    }
    return coor;
}



Coordinates MyAlgorithm::getCurrLocation() const {
    return graph.getCurrLocation();
}


int MyAlgorithm::getRemainedSteps() const {
    return remainedSteps;
}


char MyAlgorithm::getCharFromStepsLog(int index) const {
    return stepsListLog[index];
}


int MyAlgorithm::getStepsLogSize() const {
    return int(stepsListLog.size());
}


int MyAlgorithm::getTotalDirtTheRobotCleaned() const{
    return totalDirtTheRobotCleaned;
}


bool MyAlgorithm::isAtDocking() const {
    return graph.getCurrLocation() == graph.getDockingLocation();
}


bool MyAlgorithm::isCharged() const{
    return robot->isCharged();
}


bool MyAlgorithm::validLocation(Coordinates coor) const {
    char c = graph.getVal(coor);
    int rowSize = graph.getNumOfRows();
    int colSize = graph.getNumOfCols();
    int x = coor.getX();
    int y = coor.getY();
    
    bool b1 = ((c >= '0' && c <= '9') || c == ' ' || c == 'D' || c == '-'|| c == 'x');
    bool b2 = (x >= 0 && y >= 0 && x <= colSize && y <= rowSize);
    return b1 && b2;
}


void MyAlgorithm::decreaseRemainedSteps() {
    remainedSteps--;
}


void MyAlgorithm::increaseTotalDirtTheRobotCleaned() {
    graph.clean();
    totalDirtTheRobotCleaned++;
}


bool MyAlgorithm::isBatteryLow(int battery, int docking_dist){
    return (battery < docking_dist + 3);
}


Step MyAlgorithm::convertDirectionToStep(Direction d) {
    switch (d) {
        case Direction::North:
            return Step::North;
        case Direction::South:
            return Step::South;
        case Direction::West:
            return Step::West;
        case Direction::East:
            return Step::East;
        default:
            return Step::North;
    }
}


Direction MyAlgorithm::convertStepToDirection(Step s){
    switch (s) {
        case Step::North:
            return Direction::North;
        case Step::South:
            return Direction::South;
        case Step::West:
            return Direction::West;
        case Step::East:
            return Direction::East;
        default:
            return Direction::North;
    }
}


char MyAlgorithm::convertStepToChar(Step s){
    switch (s) {
        case Step::North:
            return 'N';
        case Step::South:
            return 'S';
        case Step::West:
            return 'W';
        case Step::East:
            return 'E';
        case Step::Stay:
            return 's';
        case Step::Finish:
            return 'F';
        default:
            return 's';
    }

}


// Debugging functions
void MyAlgorithm::printGraph() {
    graph.printGraph();
}


