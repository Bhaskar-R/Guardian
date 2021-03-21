#include <Arduino.h>
#include "I2CSB.h"
#include "Wire.h"

I2CSB::I2CSB(byte address)
{
  _address = address;
}

byte I2CSB::readI2Csensor(byte _address)
{
  long entry = millis();
  Wire.requestFrom((uint8_t)_address, (uint8_t)2);
  while (Wire.available() == 0 && (millis() - entry) < 100)
  {
  }
  if (millis() - entry < 100)
    return Wire.read();
}

byte I2CSB::getLevel()
{
  byte ReceivedData[3] = {0, 0, 0};
  Wire.beginTransmission(_address); // transmit to device #4
  Wire.write(0xAA);                 // sends one byte
  Wire.endTransmission();           // stop transmitting
  delay(100);
  for (int i = 0; i < 3; i++)
  {
    ReceivedData[i] = readI2Csensor(_address);
  }
  bool MSB = ReceivedData[0] == 0xAA && ReceivedData[1] == ReceivedData[2];
  byte returnVal = ReceivedData[1];
  returnVal &= ~0x80;
  returnVal |= MSB << 7;
  delay(1000);
  return returnVal;
}

bool I2CSB::setOutputs(bool LightPin, bool MistMotor, bool PlantMotor, bool BucketMotor)
{
  byte ReceivedData[3] = {0, 0, 0};
  byte sendByte = BucketMotor | PlantMotor << 1 | MistMotor << 2 | LightPin << 3;
  sendByte |= 0x60;
  Wire.beginTransmission(_address);
  Wire.write(sendByte);
  Wire.endTransmission();
  delay(1000);
  for (int i = 0; i < 3; i++)
  {
    ReceivedData[i] = readI2Csensor(_address);
  }
  delay(1000);
  return ReceivedData[0] == sendByte;
}

int I2CSB::getTemperature()
{
  byte ReceivedData[3] = {0, 0, 0};
  Wire.beginTransmission(_address);
  Wire.write(0xCA);
  Wire.endTransmission();
  delay(100);
  for (int i = 0; i < 3; i++)
  {
    ReceivedData[i] = readI2Csensor(_address);
  }
  bool crt = (ReceivedData[0] == 0xCA) && (ReceivedData[1] == ReceivedData[2]);
  delay(1000);
  if (crt)
    return map(ReceivedData[1], 100, 200, 0, 50);
  else
    return -1;
}

int I2CSB::getHumidity()
{
  byte ReceivedData[3] = {0, 0, 0};
  Wire.beginTransmission(_address);
  Wire.write(0x5C);
  Wire.endTransmission();
  delay(100);
  for (int i = 0; i < 3; i++)
  {
    ReceivedData[i] = readI2Csensor(_address);
  }
  bool crt = (ReceivedData[0] == 0x5C) && (ReceivedData[1] == ReceivedData[2]);
  delay(1000);
  if (crt)
    return map(ReceivedData[1], 100, 220, 20, 80);
  else
    return -1;
}

bool I2CSB::isConnected()
{
  byte ReceivedData[3] = {0, 0, 0};
  Wire.beginTransmission(_address); // transmit to device #4
  Wire.write(0xAA);                 // sends one byte
  Wire.endTransmission();           // stop transmitting
  delay(100);
  for (int i = 0; i < 3; i++)
  {
    ReceivedData[i] = readI2Csensor(_address);
  }
  delay(1000);
  return ReceivedData[0] == 0xAA;
}
