#include "House.h"
#include "Utils.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>



// construnctors
House::House() : maxSteps(0), maxBattery(0), rows(0), cols(0), totalDirt(0) {}

House::House(const std::string& file_path) { 
    try {
        if (!loadFromFile(file_path)) {
            throw std::runtime_error("Failed to open the input file.");
        }
    } 
    catch (const std::exception&) {
        std::cerr << "Error: can't load the house layout From the File" << std::endl;
    } 
}


// main functions
bool House::loadFromFile(const std::string& file_path) {
   try
   {
        std::ifstream file(file_path);
        if (!file) {
            return false;
        }
        std::string line;
        if (std::getline(file, line)) fileName = line;
        std::getline(file, line); 
        if (!checkParameter(line, "MaxSteps", maxSteps)) {
            throw std::runtime_error("The file does not contain MaxSteps so the run cannot be completed");
        }

        std::getline(file, line); 
        if (!checkParameter(line, "MaxBattery", maxBattery)) {
            throw std::runtime_error("The file does not contain MaxBattery so the run cannot be completed");
        }

        std::getline(file, line); 
        if (!checkParameter(line, "Rows", rows)) {
            throw std::runtime_error("The file does not contain Rows variable so the run cannot be completed");
        }
        std::getline(file, line); 
        if (!checkParameter(line, "Cols", cols)) {
            throw std::runtime_error("The file does not contain Cols variable so the run cannot be completed");
        }

        std::vector<char> row;
        while (std::getline(file, line)) {
            row = std::vector<char>(line.begin(), line.end());
            fixLine(row);
            layout.push_back(row);
        }

        // makes sure that the Layout is exactly the size rows on cols
        cutExtraWalls();
        
        // adding an outer frame of walls
        addWalls();
        
        // Find the docking station and update totalDirt
        totalDirt = 0;
        bool found_docking = false;
        for (int y = 0; y < static_cast<int>(layout.size()); ++y) {
            for (int x = 0; x < static_cast<int>(layout[y].size()); ++x) {
                if (layout[y][x] == 'D') {
                    found_docking = true;
                    DockingStation = Coordinates(x,y);
                }
                if (layout[y][x] > '0' && layout[y][x] <= '9') {
                    totalDirt += layout[y][x] - '0';
                }
                if (layout[y][x] == '0') {
                    layout[y][x] = ' ';
                }    
            }
        }
        if (!found_docking) {
            throw std::runtime_error("Docking station not found in house layout.");
        }
        file.close();
        return true;
   }

   catch(const std::exception& e)
   {
        std::filesystem::path entryPath(file_path);
        writeErrorToFile(entryPath.stem().string() + ".error", e.what());
        return false;
   }
}


// helping functions
bool House::checkParameter(const std::string line, const std::string paramName, int& param) {
    std::string firsrNewParam = paramName + " =";
    std::string secondNewParam = paramName + "=";
    try {
        if (line.find(firsrNewParam) != std::string::npos) {
            param = std::stoi(line.substr(line.find('=') + 1));
            return true;
        }
        else if (line.find(secondNewParam) != std::string::npos) {
            param = std::stoi(line.substr(line.find('=') + 1));
            return true;
        }
        else {
            std::cout << "The " << paramName << " was not provided as required by the format" << std::endl;
            return false;
        }        
    } 
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "For some reason, std::stoi function can't convert the " << paramName << " into int" << std::endl;
        return false;
    }
    return false;
}

void House::cutExtraWalls() {
    if (static_cast<int>(layout.size()) == rows) {
        return;
    }
    else if (static_cast<int>(layout.size()) > rows) {
        layout.resize(rows);
    }
    // if there are fewer rows than rows, add empty rows
    else{
        while (static_cast<int>(layout.size()) < rows) {
            std::vector<char> newRow(cols, ' ');
            layout.push_back(newRow);
        }

    }
}

void House::addWalls() {
    // add upper walls (first line) of length cols
    layout.insert(layout.begin(), std::vector<char>(cols, 'W'));
    // add buttom walls (last line) of length cols
    layout.push_back(std::vector<char>(cols, 'W'));
    // add 'W' char to the first and last char of ech line
    for (size_t i = 0; i < layout.size(); i++) {
        layout[i].insert(layout[i].begin(), 'W');
        layout[i].push_back('W');
    }
}

void House::fixLine(std::vector<char>& line) {
    // if it's an empty line replace it with ' ' line
    if (line.empty() || line[0] == '\r') {
        line = std::vector<char>(cols, ' ');
        return;
    }
    // if line.size() < cols add 'W' if  it's a only walls row or ' ' if not
    if (static_cast<int>(line.size()) < cols) {
        line.resize(cols, ' ');
    }
    // // if line.size() < cols reduce its size to cols
    if (static_cast<int>(line.size()) > cols) {
        line.resize(cols);
    }

    // Check each character and replace invalied char with ' '
    for (char& ch : line) {
        if (!(ch == 'W' || ch == 'D' || (ch >= '0' && ch <= '9') || ch == ' ')) {
            ch = ' ';
        }
    }
}



// getters implementations.
std::vector<std::vector<char>> House::getLayout() const{
    return layout;
}

int House::getMaxStepsAllowed() const{
    return maxSteps;
}

int House::getBatteryCapacity() const{
    return maxBattery;
}

int House::getRows() const{
    return rows;
}

int House::getCols() const{
    return cols;
}

int House::getTotalDirt() const {
    return totalDirt;
}

Coordinates House::getDockingCoordinates() const{
    return DockingStation;
}

char House::getLayoutVal(int x, int y) const {
    return layout[y][x];
}


// Update layout functions
void House::decreseDirtLevel(int x, int y) {
    layout[x][y]--;
    if(layout[x][y] == '0') {
        layout[x][y] = ' ';
    }
}



// print function - Debugging
void House::printDocking() {
    std::cout << "Docking Station: Layout[" << DockingStation.getX() << "][" << DockingStation.getY() << "]" << std::endl;
}

void House::printline(std::vector<char> line) {
    for (char ch : line) {
        std::cout << ch << " ";
    }
    std::cout << std::endl;
}

void House::printLayout() {
    
    std::vector<std::vector<char>> layout = getLayout();
    for (const auto& line : layout) {
        for (char ch : line) {
            std::cout << ch << " ";
        }
        std::cout << std::endl;
    }
}

void House::printTwoLayout(const std::vector<std::vector<char>> originalLayout) {
    
    std::cout << "Output file: ";
    int diff = int(layout[0].size());
    diff -= 13;
    while(diff > 0){  
        std::cout << " "; 
        diff--;
    }
    const std::string tabSpacing = "\t\t\t\t";
    std::cout << tabSpacing << "Input file:" << std::endl;
    
    std::vector<std::vector<char>> layout = getLayout();
    size_t maxRows = std::max(originalLayout.size(), layout.size());

    for (size_t i = 0; i < maxRows; ++i) {
        
        if (i < layout.size()) {
            for (char c : layout[i]) {
                std::cout << c;
            }
        }
        if(i >= layout.size()){
            for (int j = 0; j < static_cast<int>(layout[0].size()); j++) {
                std::cout << " ";
            }
        }
        std::cout << tabSpacing;
        
        if (i < originalLayout.size()) {
            for (char c : originalLayout[i]) {
                std::cout << c;
            }
        }
        std::cout << std::endl;
    }
}

