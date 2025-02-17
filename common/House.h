#ifndef HOUSE_H
#define HOUSE_H

#include <vector>
#include <string>
#include <iostream>
#include "Coordinates.h"



class House {

    private:
        std::string fileName;
        int maxSteps;
        int maxBattery;
        int rows;
        int cols;
        int totalDirt;
        Coordinates DockingStation;
        std::vector<std::vector<char>> layout;

        // helping functions
        bool checkParameter(const std::string line, const std::string paramName, int& param);
        void cutExtraWalls();
        void addWalls();
        void fixLine(std::vector<char>& line);


    public:

        // construnctor
        House ();
        House (const std::string& file_path);
        
        // functions
        bool loadFromFile(const std::string& file_path);
        
        // getters functions
        std::vector<std::vector<char>> getLayout() const;
        int getMaxStepsAllowed() const;
        int getBatteryCapacity() const;
        int getRows() const;
        int getCols() const;
        int getTotalDirt() const;
        Coordinates getDockingCoordinates() const;
        char getLayoutVal(int x, int y) const;

        // Update layout functions
        void decreseDirtLevel(int x, int y);

        
        // print functions - Debugging
        void printDocking();
        void printline(std::vector<char> line);
        void printLayout();
        void printTwoLayout(std::vector<std::vector<char>> originalLayout);

};

#endif // HOUSE_H
