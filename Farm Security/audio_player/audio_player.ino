#include <SD.h>                       // need to include the SD library
//#define SD_ChipSelectPin 53         // example uses hardware SS pin 53 on Mega2560
#define SD_ChipSelectPin 10           // using digital pin 10 on arduino uno 328, can use other pins
#define SD_SpeakerPin 9
#include <TMRpcm.h>                   // also need to include this library...
#include <SPI.h>

TMRpcm tmrpcm;

void setup(){
  tmrpcm.speakerPin = SD_SpeakerPin;            //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
                                                //Complimentary Output or Dual Speakers:
                                                //pinMode(10,OUTPUT); Pin pairs: 9,10 Mega: 5-2,6-7,11-12,46-45 
  Serial.begin(9600);
  if(!SD.begin(SD_ChipSelectPin)){
    Serial.println("SD fail");
    return;
  }
  tmrpcm.setVolume(6);
  tmrpcm.play("song.wav");
}

void loop() {
  // put your main code here, to run repeatedly:
}
