#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Graph.h"
#include "../../common/AbstractAlgorithm.h"
#include "../../common/MyWallSensor.h"
#include "../../common/MyDirtSensor.h"
#include "../../common/VacuumCleaner.h"
#include "../../common/Coordinates.h"
#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <limits>
#include <utility>
#include <map>



class MyAlgorithm : public AbstractAlgorithm {
    
    protected:
        // objects for the algorithm
        Graph graph;
        VacuumCleaner* robot;
        const WallsSensor* wallSensor;
        const DirtSensor* dirtSensor;
        const BatteryMeter* batteryMeter;
        
        // class variables
        int remainedSteps;
        std::size_t maxSteps;
        int totalDirtTheRobotCleaned = 0;
        
        // Flags
        bool isCharging = false;        // A flag that signals that the robot is chrgging
        bool goToLastspot = false;      // A flag that signals that the robot is returning to the last point it mapped
        bool robotIsDead = false;       // A flag that signals if the robot is dead
        
        //path between points in the graph
        std::vector<Direction> pathToDocking;    // steps of the way back to the Docking station.
        std::vector<Direction> pathToDirtySpot;  // steps of the way to the nearest dirty spot
        std::vector<Direction> pathToNewSpot;    // steps of the way to the pass the wall
        
        // steps history
        std::vector<char> stepsListLog;            // all the steps performed by the algorithm

    public:
        MyAlgorithm();
        virtual ~MyAlgorithm();
        void initAlgo(VacuumCleaner& robot, WallsSensor& wallSensor, DirtSensor& dirtSensor, BatteryMeter& batteryMeter);
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& sensor) override;
        void setDirtSensor(const DirtSensor& sensor) override;
        void setBatteryMeter(const BatteryMeter& meter) override;
        void setRemainedSteps(int steps);
        Step greedyStep();
        void graphReduceLines();
        void addWalls();
        Step returningToDocking();
        virtual int minDistance(int dest) = 0;
        
        // Progress functions (retrieving steps from a queue)
        Step goToDocking();
        Step goToDirtySpot();
        Step goToNewSpot();
        
        // A shadow of the robot's movement at home
        void move(Step step);
        Coordinates moveCoordinates (Coordinates coor, Direction d);
        
        // getters function
        Coordinates getCurrLocation() const;
        int getRemainedSteps() const;
        char getCharFromStepsLog(int index) const;
        int getStepsLogSize() const;
        int getTotalDirtTheRobotCleaned() const;
        bool isAtDocking() const;
        bool isCharged() const;
        bool validLocation(Coordinates coor) const;
        void decreaseRemainedSteps();
        void increaseTotalDirtTheRobotCleaned();
        bool isBatteryLow(int battery, int dist_from_docking);

        // conversion functions
        Step convertDirectionToStep(Direction d);
        Direction convertStepToDirection(Step s);
        char convertStepToChar(Step s);

        // for Debugging
        void printGraph();
};

#endif // ALGORITHM_H
