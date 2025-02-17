#include "MyBatteryMeter.h"

MyBatteryMeter::MyBatteryMeter(int batteryCapacity) : batteryCapacity(batteryCapacity), battery(batteryCapacity) {}


std::size_t MyBatteryMeter::getBatteryState() const {
    return static_cast<std::size_t>(battery);
}


void MyBatteryMeter::increaseBattery() {
    if (battery < batteryCapacity) {
        battery += batteryCapacity / 20;
        // edge case
        if (battery > batteryCapacity) {
            battery = batteryCapacity;
        }
    }
}


void MyBatteryMeter::decreaseBattery() {
    battery--;
}


bool MyBatteryMeter::isCharged() const {
    return battery == batteryCapacity;
}


void MyBatteryMeter::charge() {
    if (battery < batteryCapacity) {
        battery += (double)batteryCapacity / 20;
        // edge case
        if (battery > batteryCapacity) {
            battery = batteryCapacity;
        }
    }
}


int MyBatteryMeter::numberOfStepsToFullCharge() const {
    double steps = 0;
    int temp = battery;
    while(temp < batteryCapacity) {
        temp += (double)batteryCapacity / 20;
        steps += 1;
    }
    return steps;
}
