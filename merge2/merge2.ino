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
#define LED 13

#define RX 10                   // Connect to the TX pin of the HC-12
#define TX 11                   // Connect to the RX pin of the HC-12

#define SD_ChipSelectPin 53     // example uses hardware SS pin 53 on Mega2560
//#define SD_ChipSelectPin 10   // using digital pin 10 on arduino uno 328, can use other pins
#define SD_SpeakerPin 46        // previously was 9 for uno

#define call_owner  "ATD+ +8801857715545;"            // put owner phone number
#define sms_owner "AT+CMGS=\"+8801857715545\"\r"


DS3231  rtc(SDA, SCL);          // Init the DS3231 using the hardware interface
bool sensorValidity = true;

SoftwareSerial mySerial(RX, TX);// init software serial for hc12 communication
SoftwareSerial SIM900A(7,8);    // init software serial for gsm900

String present_time = "";       // RTC variables

int present_condition = 0;      // vibration variables
int previous_condition = 0;

TMRpcm tmrpcm;                  // needed for sd card module 


void setup(){
  Serial.begin(9600);           // initially was 115200
  mySerial.begin(115200);       // initializing software serial as well
  SIM900A.begin(9600);          // GSM Module Baud rate – communication speed

  
  //laser setup
  pinMode(DETECT, INPUT);       //define detect input pin
  pinMode(ACTION, OUTPUT);      //define ACTION output pin

  //rtc setup
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(TUESDAY);          // Set Day-of-Week to SUNDAY
  rtc.setTime(10, 25, 0);       // Set the time to 12:00:00 (24hr format)
  rtc.setDate(11, 3, 2022);     // Set the date to January 1st, 2014

  //vibration setup
  pinMode(vibration_Sensor, INPUT);
  pinMode(LED, OUTPUT);

  //sd card setup
  tmrpcm.speakerPin = SD_SpeakerPin;   //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  if(!SD.begin(SD_ChipSelectPin)){
    Serial.println("SD fail");
    return;
  }
  //tmrpcm.setVolume(6);
  //tmrpcm.play("song.wav");
}

void led_blink(void);
void vibrationCheck();
void laserCheck();
void rtcUpdate();
void timeCheck();
void hc12_Signal();
void playAudio();

void loop(){
  
  rtcUpdate();        // update from RTC
  timeCheck();        // checking the time for sensor validity 
  vibrationCheck();   // checking if vibration detected
  laserCheck();       // checking if laser interference is interfered
  hc12_Signal();      // sending signal to the receiving hc12
  
  // Wait one second before repeating loop :)
  delay (1000);       // this delay needed for rtc
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
  if((present_time[0] == '0' && present_time[1] == '7' && present_time[2] == ':') || (present_time[0] == '7' && present_time[1] == ':'))
    sensorValidity = false;
    
  //checking if it is 7 P.M.
  if((present_time[0] == '1' && present_time[1] == '9' && present_time[2] == ':'))
    sensorValidity = true;
}


//Function to check if vibration is detected
void vibrationCheck(){
  if(sensorValidity){                               // only functional when sensorValidity == true
    //vibration input
    previous_condition = present_condition;
    present_condition = digitalRead(A5);            // Reading digital data from the A5 Pin of the Arduino.
    if (previous_condition != present_condition) {
      mySerial.write("VIBRATION DETECTED");         // sending alert signal to receiving hc12
      Makecall();                                   // calling owner to alert
      playAudio();                                  // playing audio from sd card (code may be updated later)
      led_blink();
    }else{
      digitalWrite(LED, OFF);
    }
  }
}

// Function to check if laser detection is interfered
void laserCheck(){
  int detected = digitalRead(DETECT);       // read Laser sensor 
  if( detected == HIGH){
    digitalWrite(ACTION,LOW);               // set the buzzer OFF
    Serial.println("Detected!");
  }else{
    digitalWrite(ACTION,HIGH);              // Set the buzzer ON
    Serial.println("No laser");
    mySerial.write("LASER COMPROMISED");    // sending alert signal to receiving hc12
    Makecall();                             // calling owner to alert
    playAudio();                            // playing audio from sd card (code may be updated later)
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

// LED Blinking code that blinks 2 times in 1 second.
void led_blink(void) {
  digitalWrite(LED, ON);
  delay(250);
  digitalWrite(LED, OFF);
  delay(250);
  digitalWrite(LED, ON);
  delay(250);
  digitalWrite(LED, OFF);
  delay(250);
}


// All gsm functions
void SendMessage(String msg){
  Serial.println ("Sending Message please wait….");
  SIM900A.println("AT+CMGF=1");                       // Text Mode initialisation
  delay(1000);
  Serial.println ("Set SMS Number");
  //SIM900A.println("AT+CMGS=\"+8801857715545\"\r");  // Receiver’s Mobile Number
  SIM900A.println(sms_owner);
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

void Makecall(){
  Serial.println ("Makeing calling");
  delay (1000);
  //SIM900A.println("ATD+ +8801857715545;");
  SIM900A.println(call_owner);
  Serial.write ("making call");
  delay(5000);
  //Serial.write ("hanging up call");
  //SIM900A.println("ATH");                           // For call hang up

}

void Receivingcall(){
  Serial.println ("Receiving call");
  delay (1000);
  SIM900A.println("ATA");                             // Receiving Mode Enabled
  delay(1000);
  Serial.write ("receive call Sucessfully");
}
