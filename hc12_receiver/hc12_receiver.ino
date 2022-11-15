// See SoftwareSerial example 
// https://www.arduino.cc/en/tutorial/SoftwareSerialExample


//ARDUINO MEGA 2 AND 3 PINS DOES NOT SUPPORT SOFTWARE SERIAL. SO WE USED 10,11.
#include <SoftwareSerial.h>
#include <SD.h>  
#include <TMRpcm.h>                  
#include <SPI.h>
#include <stdio.h>
#define RX 2                      //Connect to the TX pin of the HC-12
#define TX 3                      //Connect to the RX pin of the HC-12
//#define RX 10                   //Connect to the TX pin of the HC-12
//#define TX 11                   //Connect to the RX pin of the HC-12

//#define SD_ChipSelectPin 53     // example uses hardware SS pin 53 on Mega2560
//#define SD_ChipSelectPin 10       // using digital pin 10 on arduino uno 328, can use other pins
//#define SD_SpeakerPin 9        

SoftwareSerial mySerial(RX, TX);
TMRpcm tmrpcm;                    // needed for sd card module 


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  //sd card setup
  //tmrpcm.speakerPin = SD_SpeakerPin;   //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
//  if(!SD.begin(SD_ChipSelectPin)){
//    Serial.println("SD fail");
//    return;
//  }
  //tmrpcm.setVolume(6);
  //tmrpcm.play("song.wav");

}

//void playAudio();


void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

//// play sd card audio 
//void playAudio(){
//  tmrpcm.setVolume(6);
//  tmrpcm.play("song.wav");
//}
