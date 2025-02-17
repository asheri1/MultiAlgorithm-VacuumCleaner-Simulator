#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "../../common/Coordinates.h"
#include "../../common/enums.h"

class Graph {

    private:
        std::vector<std::vector<char>> layout;
        Coordinates docking;
        Coordinates currLocation;

    public:
        Graph();
        void addNode(const Coordinates& coords, char value);
        void clean();
        void resize();
        void resizeN();
        void resizeS();
        void resizeW();
        void resizeE();
        bool updateLayout();
        bool updateNorthLayout();
        bool updateSouthLayout();
        bool updateWestLayout();
        bool updateEastLayout();
        bool containX();

        Coordinates getDockingLocation() const;
        Coordinates getCurrLocation() const;
        char getVal(Coordinates coor) const;
        char getVal(int x, int y) const;
        void setVal(Coordinates coor, char c);
        int getNumOfRows() const;
        int getNumOfCols() const;
        void move(Direction d);

        void printGraphStatus() const;
        void printGraph() const;
};

#endif // GRAPH_H
