#include "VacuumCleaner.h"


// constructor
VacuumCleaner::VacuumCleaner(House* house) : 
    house(house), 
    currentLocation(house->getDockingCoordinates()), 
    battery(house->getBatteryCapacity()),
    maxBattery(house->getBatteryCapacity()) {
}


void VacuumCleaner::charge() {
    battery.charge();
}


void VacuumCleaner::move(Direction d) {
    if (battery.getBatteryState() <= 0) { return; }

    switch (d) {
        case Direction::North:
            currentLocation = currentLocation.getCoordinatesN();
            break;
        case Direction::East:
            currentLocation = currentLocation.getCoordinatesE();
            break;
        case Direction::South:
            currentLocation = currentLocation.getCoordinatesS();
            break;
        case Direction::West:
            currentLocation = currentLocation.getCoordinatesW();
            break;
    }
    battery.decreaseBattery();
}


void VacuumCleaner::clean() {
    house->decreseDirtLevel(currentLocation.getY(),currentLocation.getX());
    battery.decreaseBattery();
}


bool VacuumCleaner::spotIsDirty(char ch) {
    int dirtLevel = (ch - '0');
    return dirtLevel > 0 && dirtLevel <= 9;
}


bool VacuumCleaner::isCharged() const {
    return battery.isCharged();
}


int VacuumCleaner::getBatteryLevel() const {
    return int(battery.getBatteryState());
}


int VacuumCleaner::getMaxBattery() const {
    return maxBattery;
}

Coordinates VacuumCleaner::getCurrentLocation() const {
    return currentLocation;
}

int VacuumCleaner::numberOfStepsToFullCharge() const {
    return battery.numberOfStepsToFullCharge();
}
