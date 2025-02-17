#ifndef VACUUMCLEANER_H
#define VACUUMCLEANER_H

#include "House.h"
#include "enums.h"
#include "Coordinates.h"
#include <vector>
#include "MyBatteryMeter.h"



class VacuumCleaner {
    private:
        House* house;
        Coordinates currentLocation;
        MyBatteryMeter battery;
        int maxBattery;

    public:
        // constructor
        VacuumCleaner(House* house);
        
        // functions
        void charge();
        void move(Direction d);
        void clean();
        bool spotIsDirty(char ch);
        bool isCharged() const;

        // getters
        int getBatteryLevel() const;
        int getMaxBattery() const;
        Coordinates getCurrentLocation() const;
        int numberOfStepsToFullCharge() const;
};

#endif
