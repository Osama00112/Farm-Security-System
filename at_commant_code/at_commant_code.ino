#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM900
SoftwareSerial mySerial(12, 13); //SIM900 Tx & Rx is connected to Arduino #7 & #8
String found = "";
String prev = "";
String osama;
bool check = false;
bool numCheck = false;
void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM900
  mySerial.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();
  mySerial.println("AT+CLIP=1");
  updateSerial();
  check = true;
}

void loop()
{
  //updateSerial();
  if (mySerial.available()>0){
    osama = mySerial.readString();
  }

 if(osama.indexOf("+88018577155") > -1){
    Serial.println("FOUNDDDD");
  }
}

void updateSerial()
{
  found = "";
  
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());    //Forward what Serial received to Software Serial Port
  }
  
  while(mySerial.available()) 
  {   
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
