#ifndef ADVANCED_ALGORITHM_H
#define ADVANCED_ALGORITHM_H

#include "../algo_common/MyAlgorithm.h" 

class Algorithm_A : public MyAlgorithm {

public:
    Algorithm_A();
    virtual ~Algorithm_A();

    Step nextStep() override;

    // BFS function for minimum steps until next spot and initialize the relevant path 
    // dest = 0 (for docking)
    // dest = 1 (for dirty spot) 
    // dest = 2 (for new spot)
    int minDistance(int dest) override;
    
    
    bool validLocation(Coordinates coor) const;

};

#endif // ADVANCED_ALGORITHM_H