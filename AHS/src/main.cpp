/*
Author   - Bhaskar R
Date     - 11/03/2021
Email    - bhaskar9980@gmail.com
Github   - https://github.com/Bhaskar-R
LinkedIN - https://www.linkedin.com/in/bhaskar9980/
*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // LCD Display
#include <Wire.h>              // I2C Communication
#include <RTClib.h>            // Real Time Clock
#include <DataBase.h>          // EEPROM DataBase
#include <I2CSB.h>             // Slave Board - Arduino Nano
#include <I2C_eeprom.h>        // EEPROM
#include <WiFi.h>              // WIFI Module
#include <FirebaseESP32.h>     // Firebase

//Uncomment the below line for Serial debugging
//#define Serial_Debug

#define WIFI_SSID "WIFI_username"
#define WIFI_PASSWORD "WIFI_Password"

#define FIREBASE_HOST "Enter your Database URL"        // Database URL
#define FIREBASE_AUTH "Enter your Database Secret Key" //Secret Key

#define Slave_address 0x2F
#define BucketMotorFrequency 10
#define BucketMotorDuration 5

//Defining the RTC clock
RTC_DS1307 RTC;

//Defining the I2C LiquidCrystal Display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//Instantiate the I2CSB Class
I2CSB SB1(Slave_address);

//Instantiate the EEPROM DataBase
DataBase DB;

//Defining the EEPROM
I2C_eeprom ee(0x51, I2C_DEVICESIZE_24LC512);

//FirebaseESP32 data object for data sending and receiving
FirebaseData fbdo;

//Global Variables
byte Seconds, Minutes, Hour, Date, Month;
bool DataCorrect;
bool PlantMotor;
bool BucketMotor;
bool MistMotor;
bool LightPin;
int ReservoirLevel;
int PlantBucketLevel;
int Humidity;
int Temperature;
int WifiStatus;
int DataBaseStatus;
int SlaveBoardStatus;
int DHTStatus;
byte Level;
byte DisplayState;
//uint8_t Fill[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned int InterruptTime;
int ReceivedDataProperly;

//Control Variables
int SetPlantMotor;
int SetMistMotor;
int SetBucketMotor;
int SetLight;
int SetWifi;
int DumpEEPROM;

String messageStatic = "Team REAPonROOF";

//Startup Display
void startupLCD()
{
    lcd.init();
    lcd.backlight();
    lcd.home();
    lcd.clear();
#ifdef Serial_Debug
    Serial.print("Automatic Plant Watering ...");
#endif
    lcd.print("SMART HYDROPONIC");
    lcd.setCursor(5, 1);
    lcd.print("SYSTEM");
    delay(700); //1000
}

//Displays the Status on the LCD Screen
void DisplayScreen()
{
    switch (DisplayState)
    {
    case 0:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Plant   BL - ");
        lcd.print(PlantBucketLevel);
        lcd.setCursor(0, 1);
        lcd.print(" Reserve BL - ");
        lcd.print(ReservoirLevel);
        DisplayState++;
        break;
    case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("B P M L H - ");
        lcd.print(Humidity);
        lcd.print("%");
        lcd.setCursor(0, 1);
        lcd.print(BucketMotor);
        lcd.print(" ");
        lcd.print(PlantMotor);
        lcd.print(" ");
        lcd.print(MistMotor);
        lcd.print(" ");
        lcd.print(LightPin);
        lcd.print(" ");
        lcd.print("T - ");
        lcd.print(Temperature);
        lcd.print("C");
        DisplayState++;
        break;
    case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" WiFi DB SB DHT ");
        lcd.setCursor(0, 1);
        lcd.print("  ");
        lcd.print(WifiStatus);
        lcd.print("    ");
        lcd.print(DataBaseStatus);
        lcd.print("  ");
        lcd.print(SlaveBoardStatus);
        lcd.print("  ");
        lcd.print(DHTStatus);
        lcd.print("   ");
        DisplayState++;
        break;
    case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  Time   Dt Mon ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.print(Hour % 12);
        lcd.print(":");
        lcd.print(Minutes);
        lcd.print(":");
        lcd.print(Seconds);
        lcd.setCursor(9, 1);
        lcd.print(Date);
        lcd.print("  ");
        lcd.print(Month);
        lcd.print("   ");
        DisplayState++;
        break;
    case 4:
        lcd.clear();
        lcd.setCursor(7, 0);
        lcd.print("By");
        lcd.setCursor(0, 1);
        lcd.print(messageStatic);
        DisplayState = 0;
        break;
    }
}

//Firebase -- Receive Data
void ReceiveData()
{
    if (Firebase.getInt(fbdo, "/Controls/Bucket Motor", SetBucketMotor) &&
        Firebase.getInt(fbdo, "/Controls/Light", SetLight) &&
        Firebase.getInt(fbdo, "/Controls/Mist Motor", SetMistMotor) &&
        Firebase.getInt(fbdo, "/Controls/Plant Motor", SetPlantMotor))
    {
        ReceivedDataProperly = 1;
    }
    else
    {
        ReceivedDataProperly = 0;
    }
#ifdef Serial_Debug
    Serial.println("Value read from database:");
    Serial.println(SetBucketMotor);
    Serial.println(SetLight);
    Serial.println(SetMistMotor);
    Serial.println(SetPlantMotor);
#endif
#ifdef Serial_Debug
    Serial.println("Data from database");
    Serial.println(Firebase.readStream(fbdo));
#endif
}
//Firebase -- Send Data
void SendData()
{
    int humidity = SB1.getHumidity();
    int temperature = SB1.getTemperature();

#ifdef Serial_Debug
    Serial.println("Database Humidity:");
    Serial.println(humidity);
#endif
    if (Firebase.setInt(fbdo, "/DHT Sensor Readings/Temperature/Data", temperature) &&
        Firebase.setInt(fbdo, "/DHT Sensor Readings/Humidity/Data", humidity) &&
        Firebase.setInt(fbdo, "/Status/Plant Motor", PlantMotor) &&
        Firebase.setInt(fbdo, "/Status/Bucket Motor", BucketMotor) &&
        Firebase.setInt(fbdo, "/Status/Mist Motor", MistMotor) &&
        Firebase.setInt(fbdo, "/Status/Light Pin", LightPin) &&
        Firebase.setInt(fbdo, "/Water Level/Plant Bucket", PlantBucketLevel) &&
        Firebase.setInt(fbdo, "/Water Level/Reservoir Bucket", ReservoirLevel))
    {
        DataBaseStatus = 1;
    }
    else
    {
        DataBaseStatus = 0;
#ifdef Serial_Debug
        Serial.println("Error in Sending Data to Database - ");
        Serial.println(fbdo.errorReason());
#endif
    }

    if (isnan(humidity) || isnan(temperature))
    {
        DHTStatus = 0;
#ifdef Serial_Debug
        Serial.println("Failed to read from DHT sensor!");
#endif
        return;
    }
    else
        DHTStatus = 1;
}

//Store Data in eeprom if the wifi is off
void StoreDate()
{
    if (DataBaseStatus == 0 || WifiStatus == 0)
    {
        DB.storeAt(ee, 2, Date, Hour, Minutes, Temperature, Humidity, Level);
    }
}

// Updates the time on the screen
void timeUpdate()
{
#ifdef Serial_Debug
    Serial.println("Updating time");
#endif
    DateTime now = RTC.now();
    Seconds = now.second();
    Minutes = now.minute();
    Hour = now.hour();
    Date = now.day();
    Month = now.month();
#ifdef Serial_Debug
  Serial.print(now.twelveHour();
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println("\t \t Done!");
#endif
}

void setup()
{

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500); //500
    }
    if (WiFi.status() == WL_CONNECTED)
        WifiStatus = 1;
    else
        WifiStatus = 0;
    Wire.begin();
    RTC.begin();
    startupLCD();
    //RTC.adjust(DateTime(__DATE__, __TIME__));
    delay(1500); //2000
    lcd.clear();
    ee.begin();
    Serial.begin(115200);
    Serial.println(WiFi.localIP());

    //--------- Firebase Auth -----------
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    InterruptTime = millis();
    DisplayScreen();
}
void loop()
{
    timeUpdate();

    // For LCD Display
    while (millis() - InterruptTime > 500)
    {
        DisplayScreen();
        InterruptTime = millis();
    }

    DataCorrect = 1;
    if (SB1.isConnected() == 1)
    {
        SlaveBoardStatus = 1;
        unsigned long TimeoutTrack = millis();
        do
        {
#ifdef Serial_Debug
            Serial.print("Get Level\t");
#endif
            Level = SB1.getLevel(); // x LA2 LA1 LA0 0 LB2 LB1 LB0  -- A => Plant Bucket B => Reservoir Bucket
#ifdef Serial_Debug
            Serial.println(Level, BIN);
#endif

        } while ((!(Level & 0x80)) && (millis() - TimeoutTrack < 10000));
        DataCorrect &= (millis() - TimeoutTrack < 10000);

        //Reservoir Levels
        ReservoirLevel = 0;
        if ((Level & 7) == 7)
            ReservoirLevel = 3;

        else if ((Level & 7) == 3)
            ReservoirLevel = 2;

        else if ((Level & 7) == 1)
            ReservoirLevel = 1;

        //Plant Bucket Levels
        PlantBucketLevel = 0;
        if (((Level >> 4) & 7) == 7)
            PlantBucketLevel = 3;

        else if (((Level >> 4) & 7) == 3)
            PlantBucketLevel = 2;

        else if (((Level >> 4) & 7) == 1)
            PlantBucketLevel = 1;

#ifdef Serial_Debug
        Serial.print("Plant Bucket Level \t");
        Serial.println(PlantBucketLevel);
        Serial.print("Reservoir Level \t");
        Serial.println(ReservoirLevel);
#endif
        //Bucket Motor
        if (PlantBucketLevel == 1 && BucketMotor == 0 && ReservoirLevel != 1)
        {
            BucketMotor = 1;
        }
        if ((PlantBucketLevel == 3 && BucketMotor == 1) || ReservoirLevel == 1 || ReservoirLevel == 0)
        {
            BucketMotor = 0;
        }

        //Manual Overriding of Bucket Motor
        if (SetBucketMotor == 1)
        {
            if (BucketMotor == 1)
            {
                Firebase.setString(fbdo, "/Controls Messages/Bucket Motor", "Bucket Motor is already in ON State");
            }
            else
            {
                if (PlantBucketLevel == 3)
                {
                    Firebase.setString(fbdo, "/Controls Messages/Bucket Motor", "Plant Bucket is already at Level-3");
                }
                else
                    BucketMotor = 1;
            }
            if (BucketMotor == 1)
                SetBucketMotor = 2;
        }
        if (SetBucketMotor == 0)
        {
            if (BucketMotor == 0)
            {
                Firebase.setString(fbdo, "/Controls Messages/Bucket Motor", "Bucket Motor is already in OFF State");
            }
            else
            {
                if (PlantBucketLevel == 3)
                {
                    Firebase.setString(fbdo, "/Controls Messages/Bucket Motor", "Plant Bucket is Filled Upto Level-3");
                    BucketMotor = 0;
                }
                else
                {
                    Firebase.setString(fbdo, "/Controls Messages/Bucket Motor", "Plant Bucket is not filled Upto Level-3");
                    BucketMotor = 0;
                }
            }
        }

        // Plant Motor
        if (((Minutes) % BucketMotorFrequency < BucketMotorDuration) && PlantMotor == 0)
        {
            PlantMotor = 1;
        }
        if (((Minutes) % BucketMotorFrequency >= BucketMotorDuration) && PlantMotor == 1)
        {
            PlantMotor = 0;
        }

        //Manual Overriding of Plant Motor
        if (SetPlantMotor == 1)
        {
            if (PlantMotor == 1)
            {
                Firebase.setString(fbdo, "/Controls Messages/Plant Motor", "Plant Motor is already in ON State");
            }
            else
                PlantMotor = 1;
            if (PlantMotor == 1)
                SetPlantMotor = 2;
        }
        if (SetPlantMotor == 0)
        {
            if (PlantMotor == 0)
            {
                Firebase.setString(fbdo, "/Controls Messages/Plant Motor", "Plant Motor is already in OFF State");
            }
            else
                PlantMotor = 0;
        }

        //Light
        if ((6 <= Hour) && (Hour <= 20) && LightPin == 0)
        {
            LightPin = 1;
        }
        if ((Hour >= 20) && LightPin == 1)
        {
            LightPin = 0;
        }

        //Manual Overriding of Lights
        if (SetLight == 1)
        {
            if (LightPin == 1)

            {
                Firebase.setString(fbdo, "/Controls Messages/Light", "Light is already in ON State");
            }
            else
                LightPin = 1;
            if (LightPin == 1)
                SetLight = 2;
        }
        if (SetLight == 0)
        {
            if (LightPin == 0)
            {
                Firebase.setString(fbdo, "/Controls Messages/Light", "Light is already in OFF State");
            }
            else
                LightPin = 0;
        }

        TimeoutTrack = millis();
        do
        {
#ifdef Serial_Debug
            Serial.print("Get Humidity\n");
#endif

            Humidity = SB1.getHumidity();
            delay(700); //1000

#ifdef Serial_Debug
            Serial.print("%d\n", Humidity);
#endif
        } while ((millis() - TimeoutTrack < 10000) && (Humidity == -1));
        DataCorrect &= (millis() - TimeoutTrack < 10000);

        TimeoutTrack = millis();
        do
        {
#ifdef Serial_Debug
            Serial.print("Get Temperature\n");
#endif

            Temperature = SB1.getTemperature();
            delay(700); //1000

#ifdef Serial_Debug
            Serial.print("%d\n", temperature);
#endif
        } while ((millis() - TimeoutTrack < 10000) && (Temperature == -1));
        DataCorrect &= (millis() - TimeoutTrack < 10000);

        if ((Temperature != -1) && (Humidity != -1))
        {
            //Humidity
            if (Humidity <= 60 && MistMotor == 0)
            {
                MistMotor = 1;
            }

            //Temperature
            if (Temperature >= 30 && MistMotor == 0)
            {
                MistMotor = 1;
            }
            if (Temperature < 30 && MistMotor == 1 && Humidity > 60)
            {
                MistMotor = 0;
            }
        }
        else
        {
            DataCorrect = 0;
        }

        TimeoutTrack = millis();
        do
        {
#ifdef Serial_Debug
            Serial.print("Set outputs\n");
#endif
            delay(700); //1000
        } while ((millis() - TimeoutTrack < 10000) && !SB1.setOutputs(LightPin, MistMotor, PlantMotor, BucketMotor));
        DataCorrect &= (millis() - TimeoutTrack < 10000);

        Serial.printf("B = %d, P = %d, M = %d, L = %d\n", BucketMotor, PlantMotor, MistMotor, LightPin);

        //Manual Overriding of Mist Motor
        if (SetMistMotor == 1)
        {
            if (MistMotor == 1)
            {
                Firebase.setString(fbdo, "/Controls Messages/Mist Motor", "Mist Motor is already in ON State");
            }
            else
                MistMotor = 0;
            if (MistMotor == 1)
                SetMistMotor = 2;
        }
        if (SetMistMotor == 0)
        {
            if (MistMotor == 0)
            {
                Firebase.setString(fbdo, "/Controls Messages/Mist Motor", "Mist Motor is already in OFF State");
            }
            else
                MistMotor = 0;
        }

        if (DataCorrect)
        {
#ifdef Serial_Debug
            Serial.print("All Data is correct and sending to Firebase\n\n");
#endif
            SendData();
            ReceiveData();
        }
        else
        {
#ifdef Serial_Debug
            Serial.print("Data is wrong!\n\n");
#endif
        }
    }
    else
        SlaveBoardStatus = 0;
}
