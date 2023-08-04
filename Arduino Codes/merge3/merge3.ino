#include <SoftwareSerial.h>
#include <DS3231.h>
#include <SD.h>  
#include <TMRpcm.h>                  
#include <SPI.h>
#include <stdio.h>

#define ON 1
#define OFF 0

#define DETECT 2                // pin 2 for  laser sensor detection pin
#define ACTION 8                // pin 8 for action to do someting

#define vibration_Sensor 6
#define LED 7

#define RX 10                   // Connect to the TX pin of the HC-12
#define TX 11                   // Connect to the RX pin of the HC-12

#define SD_ChipSelectPin 53     // example uses hardware SS pin 53 on Mega2560, MOSI 51, MISO 50, CLK 52
//#define SD_ChipSelectPin 10   // using digital pin 10 on arduino uno 328, can use other pins
#define SD_SpeakerPin 46        // previously was 9 for uno

#define call_owner1  "ATD+ +8801857715545;"            // put owner phone number
#define sms_owner1 "AT+CMGS=\"+8801857715545\"\r"
#define call_owner2  "ATD+ +8801765014450;"            // put owner phone number
#define sms_owner2 "AT+CMGS=\"+8801765014450\"\r"

#define MOSFET 49
#define buzzer 48

DS3231  rtc(SDA, SCL);          // Init the DS3231 using the hardware interface
bool sensorValidity = true;

SoftwareSerial mySerial(RX, TX);// init software serial for hc12 communication
SoftwareSerial SIM900A(12,13);    // init software serial for gsm900

String present_time = "";       // RTC variables
String incoming_call_string;

int present_condition = 0;      // vibration variables
int previous_condition = 0;

TMRpcm tmrpcm;                  // needed for sd card module 

int toggle = 1;
bool ifCallMade = false;

void setup(){
  pinMode(MOSFET,OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(MOSFET, HIGH);
  digitalWrite(buzzer,LOW);
  //Serial.println("inside setup");
  mySerial.begin(9600);         // initializing hc12 software serial as well
  Serial.begin(9600);           // initially was 115200 
  SIM900A.begin(9600);          // GSM Module Baud rate – communication speed

  Serial.println("Initializing...");
  delay(1000);

  SIM900A.println("AT"); //Handshaking with SIM900
  updateSerial();
  SIM900A.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  SIM900A.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  SIM900A.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();
  SIM900A.println("AT+CLIP=1");
  updateSerial();
  
  
  //laser setup
  pinMode(DETECT, INPUT);       //define detect input pin
  pinMode(ACTION, OUTPUT);      //define ACTION output pin

  //rtc setup
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(TUESDAY);          // Set Day-of-Week to SUNDAY
  rtc.setTime(5, 57, 0);       // Set the time to 12:00:00 (24hr format)
  rtc.setDate(11, 3, 2022);     // Set the date to January 1st, 2014

  //vibration setup
  pinMode(vibration_Sensor, INPUT);
  pinMode(LED, OUTPUT);

  //sd card setup
  tmrpcm.speakerPin = SD_SpeakerPin;   //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
//  if(!SD.begin(SD_ChipSelectPin)){
//    Serial.println("SD fail");
//    return;
//  }

  // no need to uncomment the lines below
  //tmrpcm.setVolume(6);
  //tmrpcm.play("song.wav");

  //Serial.println("done setting up");

}

void led_blink(void);
void vibrationCheck();
void laserCheck();
void rtcUpdate();
void timeCheck();
void hc12_Signal();
void playAudio();
void numberCheck();
void buzzingAndBlink();

void loop(){
  //Serial.println("inside loop");
  
  //rtcUpdate();        // update from RTC
  timeCheck();        // checking the time for sensor validity 
  vibrationCheck();   // checking if vibration detected, delay = 7 sec
  laserCheck();       // checking if laser interference is interfered
  //hc12_Signal();      // sending signal to the receiving hc12 (maybe not needed)
  numberCheck();      // check if owner called to turn sensor off/on


//  if (SIM900A.available()>0)
//    Serial.write(SIM900A.read());

  // Wait one second before repeating loop 🙂 
  delay (500);       // this delay needed for rtc
}

void rtcUpdate(){     // serial prints wont be needed afterwards. maybe discarded 
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  
  // Send time
  present_time = rtc.getTimeStr();    // present_time now contains string of time format. for example, present_time = "12:50:52"
  Serial.println(rtc.getTimeStr());
}

void timeCheck(){
  //checking if it is 7 A.M.
  if((present_time[0] == '0' && present_time[1] == '7' && present_time[2] == ':') || (present_time[0] == '7' && present_time[1] == ':')){
    //sensorValidity = false;
    digitalWrite(MOSFET, LOW);
    toggle = toggle * (-1);
    Serial.println("its 7 am. sensors turned OFF");
  }

  //checking if it is 7 P.M.
  if((present_time[0] == '1' && present_time[1] == '9' && present_time[2] == ':')){
    //sensorValidity = true;
    digitalWrite(MOSFET, HIGH);
    toggle = toggle * (-1);
    Serial.println("its 7 pm.sensors turned ON");
  }    
}

//Function to check if vibration is detected
void vibrationCheck(){
  //if(sensorValidity)
  {                               // only functional when sensorValidity == true
    //vibration input
    previous_condition = present_condition;
    present_condition = digitalRead(A5);            // Reading digital data from the A5 Pin of the Arduino.
    if (previous_condition != present_condition) {
      //buzzingAndBlink();
      digitalWrite(buzzer, HIGH);
      digitalWrite(LED, HIGH);
      mySerial.write("VIBRATION DETECTED");         // sending alert signal to receiving hc12
      Serial.println("VIBRATION DETECTED");
      //led_blink();                                  // blinking LED , delay = 1 sec
      Makecall(1);                                   // calling owner to alert delay = 6 sec
      //Makecall(2);
      //playAudio();                                  // playing audio from sd card (code may be updated later)

    }else{
      digitalWrite(buzzer, LOW);
      digitalWrite(LED, LOW);
      //Serial.println("VIBRATION not DETECTED");
      //digitalWrite(LED, OFF);
    }
  }

  delay(1000);
}

// Function to check if laser detection is interfered
void laserCheck(){
  int detected = digitalRead(DETECT);       // read Laser sensor 
  if(detected == HIGH){
    //digitalWrite(ACTION,LOW);               // set the buzzer OFF
    Serial.println("Detected!");
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
  }else{
    //buzzingAndBlink();
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    //digitalWrite(ACTION,HIGH);              // Set the buzzer ON
    Serial.println("No laser");
    mySerial.write("LASER COMPROMISED");    // sending alert signal to receiving hc12
    Makecall(1);                            // calling owner to alert delay = 6 sec
    //Makecall(2);                            // calling owner to alert
    //playAudio();                          // playing audio from sd card (code may be updated later)
    //digitalWrite(buzzer, LOW);
    //digitalWrite(LED, LOW);
  }
  delay(200);
}


// Function to send signal through hc12
void hc12_Signal(){
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

// play sd card audio 
void playAudio(){
  tmrpcm.setVolume(6);
  tmrpcm.play("song.wav");
}


// Verify owner phone number
void numberCheck(){
  if (SIM900A.available()>0){
    incoming_call_string = SIM900A.readString();
    Serial.println("available");
  }

  if(ifCallMade == false && (incoming_call_string.indexOf("+88018577155") > -1   || incoming_call_string.indexOf("+8801765014450") > -1) ){    // Check if the string we read had any substring containing the owner number
    Serial.println("FOUNDDDD");                                                                                                               // Makecall() func can also detect owner, we want to avoid that
    if(toggle == 1){
      digitalWrite(MOSFET, LOW);
    }
    else{
      digitalWrite(MOSFET, HIGH);
    }
    toggle = toggle * (-1);
    incoming_call_string = "";
    delay(3000);
  }
  else if(ifCallMade == true){
    Serial.println("GSM called owner at some point");
    ifCallMade = false;
    incoming_call_string = "";
  }
}


// LED Blinking code that blinks 2 times in 1 second.
void led_blink(void) {
  digitalWrite(LED, ON);
  delay(150);
  digitalWrite(LED, OFF);
  delay(150);
  digitalWrite(LED, ON);
  delay(150);
  digitalWrite(LED, OFF);
  delay(150);
}


// All gsm functions
void SendMessage(String msg){
  Serial.println ("Sending Message please wait….");
  SIM900A.println("AT+CMGF=1");                       // Text Mode initialisation
  delay(1000);
  Serial.println ("Set SMS Number");
  //SIM900A.println("AT+CMGS=\"+8801857715545\"\r");  // Receiver’s Mobile Number
  SIM900A.println(sms_owner1);
  SIM900A.println(sms_owner2);
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println(msg);
  //SIM900A.println("This is message content");       // Messsage content
  delay(100);
  Serial.println ("Done");
  SIM900A.println((char)26);// delay(1000);
  Serial.println ("Message sent succesfully");
}

void RecieveMessage(){
  Serial.println ("Receiving Messeges");
  delay (1000);
  SIM900A.println("AT+CNMI=2,2,0,0,0");               // Receiving Mode Enabled
  delay(1000);
  Serial.write ("Messege Received Sucessfully");
}

void Makecall(int num){
  Serial.println ("Calling owner");
  delay (500);  
  SIM900A.println(call_owner2);
  Serial.write ("call made  to owner 2");
  //delay(20000);
  
 // {
  //  SIM900A.println(call_owner1);
  //  Serial.write ("call made to owner 1");
//}
  //delay(5000);
  
  delay(4000);
  //Serial.write ("hanging up call");
  //SIM900A.println("ATH");                           // For call hang up
  ifCallMade = true;
}

void Receivingcall(){
  Serial.println ("Receiving call");
  delay (1000);
  SIM900A.println("ATA");                             // Receiving Mode Enabled
  delay(1000);
  Serial.write ("receive call Sucessfully");
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    SIM900A.write(Serial.read());    //Forward what Serial received to Software Serial Port
  }
  while(SIM900A.available()) 
  {   
    Serial.write(SIM900A.read());//Forward what Software Serial received to Serial Port
  }

}

void buzzingAndBlink(){
  for(int i=0; i<10; i++){
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
    delay(2000);
  }
}
