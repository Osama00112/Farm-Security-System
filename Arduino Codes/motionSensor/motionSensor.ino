int pirPin = 45;  // PIR sensor output pin

void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);  // Start the Serial Monitor
}

void loop() {
  int sensorValue = digitalRead(pirPin);

  if (sensorValue == HIGH) {
    Serial.println("Motion detected!");
  } else {
    Serial.println("No motion.");
  }

  delay(100);  // Check every 100ms
}
