#ifndef MY_BATTERY_METER_H_
#define MY_BATTERY_METER_H_

#include "House.h"
#include "enums.h"
#include "BatteryMeter.h"


class MyBatteryMeter : public BatteryMeter {

	private:
		int batteryCapacity = 0;
		double battery = 0;

	public:
		MyBatteryMeter(int batteryCapacity);
		virtual ~MyBatteryMeter() {}
		virtual std::size_t getBatteryState() const override;
		void charge();
		void increaseBattery();
		void decreaseBattery();
		bool isCharged() const;
		int numberOfStepsToFullCharge() const;
};

#endif  // MY_BATTERY_METER_H_
