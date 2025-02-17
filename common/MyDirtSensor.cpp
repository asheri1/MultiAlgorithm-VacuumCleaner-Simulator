#include "MyDirtSensor.h"


MyDirtSensor::MyDirtSensor(const House* house, const VacuumCleaner* robot) : house(house), robot(robot) {}


int MyDirtSensor::dirtLevel() const {
	
	Coordinates temp = robot->getCurrentLocation();
	char c = house->getLayoutVal(temp.getX(),temp.getY());
	int res = 0;
	if(c >='0' && c <='9') {
		res += c - '0';
	}

	return res;
}

