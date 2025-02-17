#include "MySimulator.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <future>

MySimulator::MySimulator() : algorithm(nullptr), robot(&house), wallSensor(&house, &robot), 
    dirtSensor(&house, &robot), batteryMeter(house.getBatteryCapacity()), houseFilePath(" ")
{}


bool MySimulator::readHouseFile(const std::string& houseFilePath) {
    this->houseFilePath = houseFilePath;
    return house.loadFromFile(houseFilePath);
}


void MySimulator::setAlgorithm(AbstractAlgorithm& algo) {
    algorithm = (MyAlgorithm*)&algo;
    algorithm->setRemainedSteps(house.getMaxStepsAllowed());
    robot = VacuumCleaner(&house);
    wallSensor = MyWallsSensor(&house, &robot);
    dirtSensor = MyDirtSensor(&house, &robot);
    batteryMeter = MyBatteryMeter(house.getBatteryCapacity());

    algorithm->initAlgo(robot, wallSensor, dirtSensor, batteryMeter);
}


void runVacuumCleaner(MySimulator* simulator) {
    while (simulator->algorithm->getRemainedSteps() > 0 && simulator->robot.getBatteryLevel() > 0) {
        Step step = simulator->algorithm->nextStep();
        
        // Stay/CLEAN
        if(step == Step::Stay) {
            // CHARGE
            if (simulator->algorithm->isAtDocking()) {
                simulator->robot.charge();
            } 
            // CLEAN
            else {
                simulator->robot.clean();
                simulator->algorithm->increaseTotalDirtTheRobotCleaned();
            } 
        }
        // Finish
        else if (step == Step::Finish) {
            if(simulator->robot.getBatteryLevel() > 0 || (simulator->robot.getBatteryLevel() == 0 && simulator->algorithm->isAtDocking())) {
                simulator->status = Status::FINISHED;
                break;
            }
            else {
                simulator->status = Status::DEAD;
                break;
            }
        }
        // step  == North/East/South/West
        else {
            Direction direction = simulator->algorithm->convertStepToDirection(step);
            simulator->algorithm->move(simulator->algorithm->convertDirectionToStep(direction));
        } 
        simulator->algorithm->decreaseRemainedSteps();
    }

    if (simulator->algorithm->getRemainedSteps() >= 0 && simulator->robot.getBatteryLevel() > 0 && simulator->algorithm->isAtDocking()){
        simulator->status = Status::FINISHED;
    }
    else if (simulator->robot.getBatteryLevel() == 0 && !(simulator->algorithm->isAtDocking())){
        simulator->status = Status::DEAD;
    }
    else {
        simulator->status = Status::WORKING;
    }
}


void MySimulator::run() {
    int maxSteps = house.getMaxStepsAllowed();
    int MsFactor = 2;
    int timeoutDurationMs = MsFactor * maxSteps;   // 2ms * MaxSteps

    // Lambda function of an async thread, to be monitored. 
    auto future = std::async(std::launch::async, [this] {
        runVacuumCleaner(this);
    }); 

    // Wait for the simulation to finish or timeout (whats happens first):
    if (future.wait_for(std::chrono::milliseconds(timeoutDurationMs)) == std::future_status::timeout) {
        std::cout << "Timeout reached - Simulation stopped, tid=" << gettid() << std::endl;

        // Set status to WORKING (to indicate it didn't finish).
        status = Status::WORKING;

        // Calculate timeout score
        int remainedDirt = house.getTotalDirt() - algorithm->getTotalDirtTheRobotCleaned();
        score = maxSteps * 2 + remainedDirt * 300 + 2000;
    } 
    else {
        // Simulation finished in time. write the normal output.
        calculateScore();
    }
    
    // Write output
    if (OutputRequired)
    {
        writeOutput();
    }
}


// write the output file
void MySimulator::writeOutput() {
    std::string output_dir = "outputs"; 

    if (!std::filesystem::exists(output_dir)) {
        std::filesystem::create_directory(output_dir);
    }

    std::string outputFileName = MySimulator::getOutputFileName();
    std::filesystem::path output_file_path = std::filesystem::path(output_dir) / outputFileName ;

    std::ofstream outFile(output_file_path);

    if (!outFile) {
        std::cerr << "Error opening output file: " << output_file_path << std::endl;
        return;
    }

    int totalSteps = house.getMaxStepsAllowed() - algorithm->getRemainedSteps();
    if(status==Status::FINISHED && totalSteps>0){totalSteps--;}

    int remainedDirt = house.getTotalDirt() - algorithm->getTotalDirtTheRobotCleaned();

    outFile << "NumSteps = " << totalSteps << std::endl;
    outFile << "DirtLeft = " << remainedDirt << std::endl;
    switch (status) {
        case Status::FINISHED:
            outFile << "Status = FINISHED" << std::endl;
            break;
        case Status::WORKING:
            outFile << "Status = WORKING" << std::endl;
            break;
        case Status::DEAD:
            outFile << "Status = DEAD" << std::endl;
            break;
    }

    std::string isAtDock = algorithm->isAtDocking() ? "True":"False";
    outFile << "inDock = " << isAtDock << std::endl;
    outFile << "Score = " << score << std::endl;

    // add a print of the steps list
    outFile << "Steps:" << std::endl;
    for(int i = 0; i < algorithm->getStepsLogSize(); i++) {
        char c = algorithm->getCharFromStepsLog(i);
        outFile << c;
    }
    outFile << std::endl;
    outFile.close();
}


std::string MySimulator::getOutputFileName() const {
    // extract house name from the file path
    size_t pos = houseFilePath.find_last_of("/\\");
    std::string houseName = (pos == std::string::npos) ? houseFilePath : houseFilePath.substr(pos + 1);

    // remove the extension from the house name
    size_t dotPos = houseName.find_last_of('.');
    if (dotPos != std::string::npos) {
        houseName = houseName.substr(0, dotPos);
    }

    return houseName + "-" + algoName + ".txt";
}


// for Debugging
void MySimulator::printLocation() {
    Coordinates currentLocation = robot.getCurrentLocation();
    //algorithm->printPathToDocking();
    //algorithm->printPathToDirtySpot();
    int x = currentLocation.getX();
    int y = currentLocation.getY();
    std::cout << "currentLocation: layout[" << x << "][" << y << "] = " << house.getLayoutVal(x, y) << "\n";
}   


void MySimulator::printLayout() {
    house.printLayout();
}


void MySimulator::printStepStatus() {
    int number_of_steps = house.getMaxStepsAllowed() - algorithm->getRemainedSteps();
    printLocation();
    std::cout << "number of steps so far = " << number_of_steps << std::endl;
    std::cout << "remainedSteps = " << algorithm->getRemainedSteps() << std::endl;
    // std::cout << "totalDirt = " << algorithm->getToatalDirt() << std::endl;
    std::cout << "battery = " << robot.getBatteryLevel() << std::endl;
    printLayout();
    std::cout << "\n" << std::endl;

}


void MySimulator::printGraphStatus(int remainedSteps, int battery) {
    std::cout << "remainedSteps = " << remainedSteps << std::endl;
    std::cout << "battery = " << battery << std::endl;
    //algorithm->printPathToDocking();
    //algorithm->printPathToDirtySpot();
    algorithm->printGraph();
}


void MySimulator::calculateScore() {
    int totalSteps = house.getMaxStepsAllowed() - algorithm->getRemainedSteps();
    int remainedDirt = house.getTotalDirt() - algorithm->getTotalDirtTheRobotCleaned();

    if (status == Status::FINISHED && totalSteps>0 ){totalSteps--;}
    switch (status) {
        case Status::DEAD:
            // score if robot is dead
            score = house.getMaxStepsAllowed() + remainedDirt * 300 + 2000;
            break;

        case Status::FINISHED:
            // score if robot reported finished
            if (algorithm->isAtDocking()) {
                score = totalSteps + remainedDirt * 300;
            } else {
                score = house.getMaxStepsAllowed() + remainedDirt * 300 + 3000;
            }
            break;

        default:
            score = totalSteps + remainedDirt * 300;
            if (algorithm->isAtDocking()){score+= 1000;}
            break;
    }
}


double MySimulator::getScore() const{
    return score;
}


void MySimulator::onlySummaryMode(){
    OutputRequired = false; 
}


void MySimulator::setAlgoName(std::string algorithmName){
    algoName = algorithmName;
}
