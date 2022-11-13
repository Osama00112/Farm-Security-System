// See SoftwareSerial example 
// https://www.arduino.cc/en/tutorial/SoftwareSerialExample


//ARDUINO MEGA 2 AND 3 PINS DOES NOT SUPPORT SOFTWARE SERIAL. SO WE USED 10,11.
#include <SoftwareSerial.h>
//#define RX 2 //Connect to the TX pin of the HC-12
//#define TX 3 //Connect to the RX pin of the HC-12
#define RX 10 //Connect to the TX pin of the HC-12
#define TX 11 //Connect to the RX pin of the HC-12
SoftwareSerial mySerial(RX, TX);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

}

void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
