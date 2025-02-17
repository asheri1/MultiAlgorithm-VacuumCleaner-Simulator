#ifndef ALGORITHM2_H
#define ALGORITHM2_H

#include "../algo_common/MyAlgorithm.h"


class Algorithm_B : public MyAlgorithm{
    
    private:
        // class variables
	    int maxRadiusSoFar = 0;
        
        // Flags
        bool moveInCircleStuck = false; // A flag that signals that the robot can't move in circle way
	    bool finishedMapping = false;   // A flag that signals if the robot has finished mapping the house
        
        //path between points in the graph
        std::vector<Direction> pathToLastMappingSpot; // steps of the way to the last mapping spot
        std::vector<Direction> pathToNewRadius;       // steps of the way to the pass the wall
	    char flagChar = 'x';    // a char that helps the robot mark points in the graph

    public:
        Algorithm_B();
        virtual ~Algorithm_B();
        Step nextStep() override;
        int calcRadiusX(int x) const;
        int calcRadiusY(int y) const;
        int calcRadius(int x, int y);
        Direction moveInCircle(Coordinates temp);
        void finishedMappingTheHouse();
        Step returningToDocking();
        int searchLargerRadius();
        
        // Progress functions (retrieving steps from a queue)
        Step goToLastMappingSpot();
        Step goToNewRadius();
        int minDistance(int dest);
        bool validLocation(Coordinates coor) const;

};
#endif // ALGORITHM_H
