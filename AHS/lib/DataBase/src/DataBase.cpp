#include "Arduino.h"
#include "Wire.h"
#include "I2C_eeprom.h"
#include "DataBase.h"

void DataBase::storeAt(I2C_eeprom ee, int Address, int Day, int Hour, int Minute, int Temperature, int Humidity, byte LevelArray)
{
    byte StoreArray[5];
    unsigned int state_store = Day * 1440 + Hour * 60 + Minute;
    StoreArray[0] = highByte(state_store);
    StoreArray[1] = lowByte(state_store);
    StoreArray[2] = map(Temperature, 0, 50, 100, 200);
    StoreArray[3] = map(Humidity, 20, 80, 100, 220);
    StoreArray[4] = LevelArray;
    for (int i = 0; i < 5; i++)
    {
        ee.writeByte(Address, StoreArray[i]);
        Address++;
    }
    ee.writeByte(0, highByte(Address));
    ee.writeByte(1, lowByte(Address));
    delay(10);
}

void DataBase::resetAddress(I2C_eeprom ee)
{
    ee.writeByte(0, 0);
    ee.writeByte(1, 2); // to point to the first dataset (at address 0x0002)
    delay(10);
}

int DataBase::getDataAt(I2C_eeprom ee, int Address, byte Data)
{
    byte AccessData[2];
    AccessData[0] = ee.readByte(Address);
    AccessData[1] = ee.readByte(Address + 1);
    unsigned int Value = AccessData[0] << 8 | AccessData[1];
    int returnVal = 0;
    if (Data == 1)
    {
        //returnVal = 1;
        returnVal = (uint16_t)Value / (uint16_t)1440;
    }
    else if (Data == 2)
    {
        //returnVal = 2;
        returnVal = ((uint16_t)Value % (uint16_t)1440) / (uint16_t)60;
    }
    else if (Data == 3)
    {
        //returnVal = 3;
        returnVal = (((uint16_t)Value % (uint16_t)1440) % (uint16_t)60);
    }
    else if (Data == 4)
    {
        //returnVal = 4;
        returnVal = map(ee.readByte(Address + 2), 100, 200, 0, 50);
    }
    else if (Data == 5)
    {
        //returnVal = 5;
        returnVal = map(ee.readByte(Address + 3), 100, 220, 20, 80);
    }
    else if (Data == 6)
    {
        //returnVal = 6;
        returnVal = ee.readByte(Address + 4);
    }
    return returnVal;
}

int DataBase::getNextAddress(I2C_eeprom ee)
{
    int ReturnVal = (ee.readByte(0) << 8) | (ee.readByte(1)); // 16 bit address
    delay(10);
    return ReturnVal;
}