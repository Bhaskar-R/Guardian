#define SensorOut 13

const int Level1[3] = {A0, A1, A2};
const int Level2[3] = {A3, A6, A7};

void setup() {
  for(int i = 0; i<3; i++)
  pinMode(Level1[i], INPUT);
  for(int i = 0; i<3; i++)
  pinMode(Level2[i], INPUT);
}

void loop() {
  Serial.print("Sensor1: ");
  for(int i = 0; i<3; i++){
    Serial.print(analogRead(Level1[i]));
    Serial.print("\t");
  }
  Serial.println();
  
  Serial.print("Sensor1: ");
  for(int i = 0; i<3; i++){
    Serial.print(analogRead(Level2[i]));
    Serial.print("\t");
  }
  Serial.println();

}
