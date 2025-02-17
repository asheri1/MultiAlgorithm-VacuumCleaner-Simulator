# Makefile for Vacuum Cleaner Simulation Project

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g -fPIC
SOURCES =  main.cpp common/Utils.cpp algorithm/algo_common/Graph.cpp algorithm/algo_common/MyAlgorithm.cpp common/Coordinates.cpp common/House.cpp simulator/MySimulator.cpp simulator/AlgorithmRegistrar.cpp common/MyBatteryMeter.cpp common/MyDirtSensor.cpp common/MyWallSensor.cpp common/VacuumCleaner.cpp
SOURCES_ALGO_A =  algorithm/Algorithm1/Algorithm_A.cpp algorithm/algo_common/Graph.cpp algorithm/algo_common/MyAlgorithm.cpp common/Coordinates.cpp common/House.cpp common/MyBatteryMeter.cpp common/MyDirtSensor.cpp common/MyWallSensor.cpp common/VacuumCleaner.cpp algorithm/AlgorithmRegistration.h
SOURCES_ALGO_B =  algorithm/Algorithm2/Algorithm_B.cpp algorithm/algo_common/Graph.cpp algorithm/algo_common/MyAlgorithm.cpp common/Coordinates.cpp common/House.cpp common/MyBatteryMeter.cpp common/MyDirtSensor.cpp common/MyWallSensor.cpp common/VacuumCleaner.cpp algorithm/AlgorithmRegistration.h
EXECUTABLE = myrobot


all: $(EXECUTABLE) algo1 algo2

$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) -rdynamic $(SOURCES) -o $(EXECUTABLE)

algo1: $(SOURCES_ALGO_A)
	$(CXX) $(CXXFLAGS) -shared -o so_files/Algorithm_A.so $(SOURCES_ALGO_A)

algo2: $(SOURCES_ALGO_B)
	$(CXX) $(CXXFLAGS) -shared -o so_files/Algorithm_B.so $(SOURCES_ALGO_B)

clean:
	rm -f $(EXECUTABLE) so_files/aAlgorithm_A.so so_files/Algorithm_B.so

.PHONY: all clean
