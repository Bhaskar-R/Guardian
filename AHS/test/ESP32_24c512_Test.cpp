#include "Wire.h"
#include "I2C_eeprom.h"


I2C_eeprom ee(0x50, I2C_DEVICESIZE_24LC512);

void setup(){
  Serial.begin(115200); 
  while (!Serial);
  ee.begin();
  byte j = 200;
  for(int i = 0; i < 100; i++){
    ee.writeByte(i, 2);
    j--;
  }
  delay(50);
  for(int i = 0; i < 100; i++){
    Serial.println(ee.readByte(i));
  }
}

void loop(){
  
}

void storeData(){
  
}
