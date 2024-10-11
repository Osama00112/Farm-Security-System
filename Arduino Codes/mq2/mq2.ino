#include <MQ2.h>

int digitalPin = 7;     // Digital pin for MQ2 sensor
int Analog_Input = A0;  // Analog pin for MQ2 sensor (optional for analog readings)
int lpg, co, smoke;

MQ2 mq2(Analog_Input);  // MQ2 sensor on analog pin A0

void setup() {
  Serial.begin(9600);   // Start serial communication
  pinMode(digitalPin, INPUT); // Set the digital pin as input
  mq2.begin();          // Initialize the MQ2 sensor
}

void loop() {
  // Read the digital pin to check for gas detection threshold
  int gasDetected = digitalRead(digitalPin);

    // Optionally, you can still read analog values for LPG, CO, and smoke
  lpg = mq2.readLPG();    // Read LPG concentration
  co = mq2.readCO();      // Read CO concentration
  smoke = mq2.readSmoke();// Read Smoke concentration

  if (gasDetected == HIGH) {
    Serial.println("Gas detected! Checking levels...");
    



  } else {
    Serial.println("No gas detected.");
  }

    // Print the sensor readings in the Serial Monitor
  Serial.print("LPG: ");
  Serial.print(lpg);
  Serial.print(" ppm | CO: ");
  Serial.print(co);
  Serial.print(" ppm | Smoke: ");
  Serial.print((smoke * 100) / 1000000);
  Serial.println(" %");

  delay(200);  // Delay for 1 second
}
