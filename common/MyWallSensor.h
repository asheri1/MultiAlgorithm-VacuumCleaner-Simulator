#ifndef MY_WALL_SENSOR_H_
#define MY_WALL_SENSOR_H_

#include "House.h"
#include "VacuumCleaner.h"
#include "WallSensor.h"


class MyWallsSensor : public WallsSensor{
	
	private:
		const House* house;
		const VacuumCleaner* robot;

	public:
		MyWallsSensor(const House* house, const VacuumCleaner* robot);
		virtual ~MyWallsSensor() {}
		virtual bool isWall(Direction d) const override;
};


#endif  // MY_WALL_SENSOR_H_

