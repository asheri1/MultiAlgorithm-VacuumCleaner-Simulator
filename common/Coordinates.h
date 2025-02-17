#ifndef COORDINATES_H
#define COORDINATES_H

#include "enums.h"

class Coordinates {
    
    private:
        int xValue;
        int yValue;

    public:
        // constructor
        Coordinates(int x=0, int y=0); // default constractor (0,0)
        
        // getters
        int getX() const;
        int getY() const;

        Coordinates getCoordinatesN() const;
        Coordinates getCoordinatesE() const;
        Coordinates getCoordinatesS() const;
        Coordinates getCoordinatesW() const;

        void setX(const int x);
        void setY(const int y);
        void setCoordinates(const Coordinates& coor);
        Coordinates move(Direction d);

        // equality operator
        bool operator==(const Coordinates& other) const;
        bool operator<(const Coordinates& other) const;
};

#endif 

