#include <Arduino.h>
#include <Wire.h>
#include "DHT.h"

//Uncomment the below line to debug
//#define Debug

#define Bucket_Motor 10
#define Plant_Motor 9
#define Mist_Motor 7
#define Light_pin 6
#define DHTPIN 4
#define Data_out 13
#define Threshold 600
#define DHTTYPE DHT11

byte c;

int sendState = 0;
#define Slave_address 0x2F

bool flag;
byte sendBytes[3];
bool Levels_bool[6];
const int Level_pins[6] = {A0, A1, A2, A3, A6, A7};
int Temperature, Humidity;

int getHumidity();
int getTemp();
void receiveEvent(int);
void requestEvent();
DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  Serial.begin(9600);
#ifdef Debug
  Serial.println("Debugging serial has started");
#endif
  pinMode(Bucket_Motor, OUTPUT);
  pinMode(Plant_Motor, OUTPUT);
  pinMode(Mist_Motor, OUTPUT);
  pinMode(Light_pin, OUTPUT);
  pinMode(Data_out, OUTPUT);
  dht.begin();
  for (int i = 0; i < 6; i++)
    pinMode(Level_pins[i], INPUT);

  Wire.begin(Slave_address);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Humidity = dht.readHumidity();
  Temperature = dht.readTemperature();
#ifdef Debug
  Serial.println("Temperature read");
#endif
}
unsigned long PreviousMillis;
void loop()
{
  if (millis() - PreviousMillis > 60000)
  {
    PreviousMillis = millis();
    Humidity = dht.readHumidity();
    Temperature = dht.readTemperature();
#ifdef Debug
    Serial.println("Temperature readed");
#endif
  }
  if (flag)
  {
    sendState = 0;
    long entry = millis();
    while (Wire.available() > 0 && (millis() - entry < 100))
    {
      c = Wire.read();
#ifdef Debug
      Serial.print("Received = 0x");
      Serial.println(c, HEX);
#endif
    }
    int Select = c >> 4;
#ifdef Debug
    Serial.print("Case = ");
    Serial.println(Select);
#endif

    if (Select == 5)
    {
      sendBytes[0] = c;
      sendBytes[1] = map(dht.readHumidity(), 20, 80, 100, 220);
      sendBytes[2] = sendBytes[1];
#ifdef Debug
      Serial.println("******************Get Humidity****************");
      Serial.print("Read Humidity = ");
      Serial.print(Humidity);
      Serial.print("\t Packed byte = ");
      Serial.println(sendBytes[1]);
#endif
    }

    else if (Select == 6)
    {
      digitalWrite(Bucket_Motor, (c)&0x01);
      digitalWrite(Plant_Motor, (c >> 1) & 0x01);
      digitalWrite(Mist_Motor, (c >> 2) & 0x01);
      digitalWrite(Light_pin, (c >> 3) & 0x01);
      sendBytes[0] = 0x60;
      sendBytes[0] |= digitalRead(Bucket_Motor);
      sendBytes[0] |= digitalRead(Plant_Motor) << 1;
      sendBytes[0] |= digitalRead(Mist_Motor) << 2;
      sendBytes[0] |= digitalRead(Light_pin) << 3;
#ifdef Debug
      Serial.println("*********Toggle Outputs***********");
      Serial.println("Buck\tPlant\tMist\tLight");
      Serial.print((c)&0x01);
      Serial.print("\t");
      Serial.print((c >> 1) & 0x01);
      Serial.print("\t");
      Serial.print((c >> 2) & 0x01);
      Serial.print("\t");
      Serial.println((c >> 3) & 0x01);
      Serial.print("Send byte = ");
      Serial.println(sendBytes[0], HEX);
#endif
      sendBytes[1] = sendBytes[0];
      sendBytes[2] = sendBytes[1];
    }
    else if (Select == 10)
    {
      digitalWrite(Data_out, HIGH);
      sendBytes[0] = c;
#ifdef Debug
      Serial.println("*********Read Levels***********");
#endif
      for (int i = 0; i < 6; i++)
      {
        Levels_bool[i] = analogRead(Level_pins[i]) > Threshold;
        delay(100);
#ifdef Debug
        Serial.print(Levels_bool[i]);
        Serial.print("\t");
#endif
      }
#ifdef Debug
      Serial.println();
#endif

      sendBytes[1] = 0;
      sendBytes[2] = 0;

      for (int i = 0; i < 3; i++)
        sendBytes[1] |= Levels_bool[i] << i;

      for (int i = 3; i < 6; i++)
        sendBytes[1] |= Levels_bool[i] << (i + 1);

      sendBytes[2] = sendBytes[1];
#ifdef Debug
      Serial.print("Packed byte = ");
      Serial.println(sendBytes[1], BIN);
#endif
      digitalWrite(Data_out, LOW);
    }
    if (Select == 12)
    {
      sendBytes[0] = c;
      sendBytes[1] = map(dht.readTemperature(), 0, 50, 100, 200);
#ifdef Debug
      Serial.println("******************Get Temperature*************");
      Serial.print("Read temperature = ");
      Serial.print(Temperature);
      Serial.print("\t Packed byte = ");
      Serial.println(sendBytes[1]);
#endif
      sendBytes[2] = sendBytes[1];
    }
    flag = 0;
  }
}
void receiveEvent(int)
{
  flag = 1;
}
void requestEvent()
{
  if (sendState == 3)
    sendState = 0;
  Wire.write(sendBytes[sendState]);
  sendState++;
}