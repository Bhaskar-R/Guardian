#define Bucket_Motor  10
#define Plant_Motor   9
#define Mist_Motor    7


void setup() {
 pinMode(Bucket_Motor, OUTPUT);
  pinMode(Plant_Motor, OUTPUT);
  pinMode(Mist_Motor, OUTPUT);
  pinMode(Light_pin, OUTPUT);

}

void loop() {
  delay(1000);
  digitalWrite(Bucket_Motor, HIGH);
  delay(1000);
  digitalWrite(Bucket_Motor, LOW);
  delay(1000);
  digitalWrite(Plant_Motor, HIGH);
  delay(1000);
  digitalWrite(Plant_Motor, LOW);
  delay(1000);
  digitalWrite(Mist_Motor, HIGH);
  delay(1000);
  digitalWrite(Mist_Motor, LOW);
  delay(1000);
  digitalWrite(Light_pin, HIGH);
  delay(1000);
  digitalWrite(Light_pin, LOW);
  delay(1000);

}
