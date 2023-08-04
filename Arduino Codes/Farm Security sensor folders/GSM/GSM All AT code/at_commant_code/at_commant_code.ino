#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM900
SoftwareSerial mySerial(12, 13); //SIM900 Tx & Rx is connected to Arduino #7 & #8
String found = "";
String prev = "";
String osama;
bool check = false;
bool numCheck = false;
bool foundStatus = false;

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
    Serial.println("available");
  }
//  else
//    foundStatus = false;

 if(/*foundStatus == false &&*/ (osama.indexOf("88018577155") > -1   || osama.indexOf("8801765014450") > -1)){
    Serial.println("FOUNDDDD");
    foundStatus = true;
    
    osama = "";
  }
//  else if (foundStatus == true && !(osama.indexOf("88018577155") > -1   || osama.indexOf("8801765014450") > -1)){
//    foundStatus = false;
//  }


  //Makecall();
}

void updateSerial()
{
  found = "";
  
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());    //Forward what Serial received to Software Serial Port
//    char c = Serial.read();
//    mySerial.write(c);
//    found += c;
//    if(found == "OK")
//      mySerial.write("clip found\n");
//    mySerial.write("\n string found: ");
//    //mySerial.write(found);
    
  }

  
  while(mySerial.available()) 
  {   
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
//    String osama = mySerial.readString();
//    Serial.print("printing string: ");
//    Serial.println(osama);
//    char c = mySerial.read();
//    Serial.write(c);
//    found += c;
//    if(c == '8' && check == true){
//      
//      numCheck = true;
//      Serial.println("plus found");
//    }
//    if(numCheck){
//      prev += c;
//    }
//    if(prev == "88"){
//      Serial.println("clip found");
//    }
    
      
//    Serial.print("\n string found: ");
//    Serial.println(found);
  
//  if(check && c == "+"){
//    Serial.write("plus found");
//  }
//    
  }
  //Serial.println(prev);
  //prev = found;
  //Serial.println("found : " + found);
}


void Makecall()
{
Serial.println ("Makeing calling");
delay (1000);
mySerial.println("ATD+ +8801857715545;");
delay(10000);
Serial.write ("make call");
mySerial.println("ATH"); //For call hang up

//
//Serial.println ("Makeing calling");
//delay (1000);
//mySerial.println("ATD+ +8801765014450;");
//delay(10000);
//Serial.write ("make call");
//mySerial.println("ATH"); //For call hang up

}
