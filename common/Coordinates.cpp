#include "enums.h"
#include "Coordinates.h"
#include <iostream>

Coordinates::Coordinates(int x, int y): xValue(x), yValue(y) {}

int Coordinates::getX() const {return xValue;}
int Coordinates::getY() const {return yValue;}


Coordinates Coordinates::getCoordinatesN() const{
    return Coordinates(xValue, yValue-1);
}

Coordinates Coordinates::getCoordinatesE() const{
    return Coordinates(xValue+1, yValue);
}

Coordinates Coordinates::getCoordinatesS() const{
    return Coordinates(xValue, yValue+1);
}

Coordinates Coordinates::getCoordinatesW() const{
    return Coordinates(xValue-1, yValue);
}

void Coordinates::setX(const int x) {
    xValue = x;
}

void Coordinates::setY(const int y) {
    yValue = y;
}

void Coordinates::setCoordinates(const Coordinates& coor) {
    xValue = coor.getX();
    yValue = coor.getY();
}

Coordinates Coordinates::move(Direction d) { 
    switch (d) {
        case Direction::North:
            return getCoordinatesN();
        case Direction::South:
            return getCoordinatesS();
        case Direction::West:
            return getCoordinatesW();
        case Direction::East:
            return getCoordinatesE();
    }
    return getCoordinatesN();
}

// equality operator
bool Coordinates::operator==(const Coordinates& other) const {
    return (xValue == other.getX() && yValue == other.getY());
}

bool Coordinates::operator<(const Coordinates& other) const {
        if (xValue == other.xValue) {
            return yValue < other.yValue;
        }
        return xValue < other.xValue;
    }