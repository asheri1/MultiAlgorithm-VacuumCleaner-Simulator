#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H

#include "../common/House.h"
#include "../common/AbstractAlgorithm.h"
#include "../algorithm/algo_common/MyAlgorithm.h"
#include "../common/VacuumCleaner.h"
#include "../common/MyBatteryMeter.h"
#include "../common/MyWallSensor.h"
#include "../common/MyDirtSensor.h"
#include "../common/enums.h"
#include <fstream>



enum Status { FINISHED, WORKING, DEAD };


class MySimulator {
    private:
        House house;
        MyAlgorithm* algorithm;
        std::string algoName;
        VacuumCleaner robot; 
        MyWallsSensor wallSensor;
        MyDirtSensor dirtSensor; 
        MyBatteryMeter batteryMeter;
        const std::string outputFilePath;
        std::vector<Step> stepsLog;
        Status status = WORKING;
        std::string houseFilePath;
        double score;
        bool OutputRequired = true; // default value.

        friend void runVacuumCleaner(MySimulator* simulator);

    public:
        MySimulator();
        bool readHouseFile(const std::string& houseFilePath);
        void setAlgorithm(AbstractAlgorithm& algo);
        void run();
        void writeOutput();
        std::string getOutputFileName() const;
        
        // print function - Debugging
        void printLocation();
        void printLayout();
        void printStepStatus();
        void printGraphStatus(int remainedSteps, int battery);
        
        void calculateScore();
        double getScore() const;
        void onlySummaryMode();
        void setAlgoName(std::string algorithmName);
};

#endif // MYSIMULATOR_H

