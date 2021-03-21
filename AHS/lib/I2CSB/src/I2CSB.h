#ifndef I2CSB_h
#define I2CSB_h

#include "Arduino.h"
#include "Wire.h"
class I2CSB
{
public:
	I2CSB(byte address);
	byte getLevel();
	bool setOutputs(bool LightPin, bool MistMotor, bool PlantMotor, bool BucketMotor);
	int getHumidity();
	int getTemperature();
	bool isConnected();

private:
	byte readI2Csensor(byte _address);
	byte _address;
};

#endif
