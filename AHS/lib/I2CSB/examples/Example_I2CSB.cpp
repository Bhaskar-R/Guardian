#include <Arduino.h>
#include <Wire.h>
#include <I2CSB.h>

#define Slave_address 0x2F

I2CSB SB1(Slave_address);

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Started");
}

void loop()
{
  Serial.println(SB1.isConnected());
  delay(2000);
  Serial.println(SB1.getLevel(), BIN);
  delay(2000);
  Serial.println(SB1.getTemperature());
  delay(2000);
  Serial.println(SB1.getHumidity());
  delay(2000);
  Serial.println(SB1.setOutputs(1, 1, 1, 1));
  delay(2000);
}
