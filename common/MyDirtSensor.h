#ifndef MY_DIRT_SENSOR_H_
#define MY_DIRT_SENSOR_H_

#include "House.h"
#include "VacuumCleaner.h"
#include "enums.h"
#include "Coordinates.h"
#include "DirtSensor.h"


class MyDirtSensor : public DirtSensor {
	
	private:
		const House* house;
		const VacuumCleaner* robot;

	public:
		MyDirtSensor(const House* house, const VacuumCleaner* robot);
		virtual ~MyDirtSensor() {}
		virtual int dirtLevel() const override;
};


#endif  // MY_DIRT_SENSOR_H_
