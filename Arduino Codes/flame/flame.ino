#define Sone 30
#define Stwo 31
#define Sthree 32
#define Sfour 33
#define Sfive 34

#define LED1 7
#define LED2 8
#define LED3 9
#define LED4 10
#define LED5 11

void setup() {
  Serial.begin(9600);
  pinMode(Sone, INPUT);
  pinMode(Stwo, INPUT);
  pinMode(Sthree, INPUT);
  pinMode(Sfour, INPUT);
  pinMode(Sfive, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
}

void loop() {
  bool value1 = digitalRead(Sone);
  bool value2 = digitalRead(Stwo);
  bool value3 = digitalRead(Sthree);
  bool value4 = digitalRead(Sfour);
  bool value5 = digitalRead(Sfive);

  //Serial.println(value5);
  if (value1 == 1 || value2 == 1 || value3 == 1 || value4 ==1 || value5 == 1) {
    digitalWrite(LED1, HIGH);
    Serial.println("Flame detected");
  } else {
    digitalWrite(LED1, LOW);
  }
//  if (value2 == 1) {
//    digitalWrite(LED2, HIGH);
//  } else {
//    digitalWrite(LED2, LOW);
//  }
//  if (value3 == 1) {
//    digitalWrite(LED3, HIGH);
//  } else {
//    digitalWrite(LED3, LOW);
//  }
//  if (value4 == 1) {
//    digitalWrite(LED4, HIGH);
//  } else {
//    digitalWrite(LED4, LOW);
//  }
//  if (value5 == 1) {
//    digitalWrite(LED5, HIGH);
//  } else {
//    digitalWrite(LED5, LOW);
//  }
}
