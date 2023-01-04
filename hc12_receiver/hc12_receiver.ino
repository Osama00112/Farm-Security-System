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
#define yellowLED 7
#define redLED 8
#define pushButton 6

//#define SD_ChipSelectPin 53     // example uses hardware SS pin 53 on Mega2560
#define SD_ChipSelectPin 4       // using digital pin 10 on arduino uno 328, can use other pins
#define SD_SpeakerPin 9        

SoftwareSerial mySerial(RX, TX);
TMRpcm tmrpcm;                    // needed for sd card module 
String found = "";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(yellowLED, OUTPUT);     // setting the pins as digital output pins (working as alert LED's)
  pinMode(redLED, OUTPUT);
  pinMode(pushButton, INPUT);     // setting pushButton pin as input (when owner pushes a button, allert LED's are turned off

  digitalWrite(yellowLED, LOW);   // at first there is no alert, hence setting their digital pins low
  digitalWrite(redLED, LOW);

  //sd card setup
  tmrpcm.speakerPin = SD_SpeakerPin;   //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  if(!SD.begin(SD_ChipSelectPin)){
    Serial.println("SD fail");
    return;
  }
//  tmrpcm.setVolume(6);
//  tmrpcm.play("song.wav");
//  Serial.println("song played");

}

void playAudio();
void turnOffLED();


void loop() { // run over and over
  if (mySerial.available()) {
    found = mySerial.readString();
    Serial.println(found);
    //Serial.write(mySerial.read());

    if(found.indexOf("VIBRATION DETECTED") > -1){
      digitalWrite(yellowLED, HIGH);              // vibration detected, turn yellow led on
      playAudio();
    }else if(found.indexOf("LASER COMPROMISED") > -1){
      digitalWrite(redLED, HIGH);                 // laser is compromised, turn red led on
      playAudio();
    }
    found = "";
    
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }


  //turnOffLED();
}

// play sd card audio 
void playAudio(){
  tmrpcm.setVolume(6);
  tmrpcm.play("song.wav");
}


void turnOffLED(){  
  if(digitalRead(pushButton) == HIGH){          // turning LED's off when button pushed    
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);  
  }
}
