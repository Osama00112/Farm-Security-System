#include <SoftwareSerial.h>
#include <DS3231.h>
#include <SD.h>  
#include <TMRpcm.h>                  
#include <SPI.h>
#include <stdio.h>

#include <MQ2.h>


#define ON 1
#define OFF 0

#define DETECT 2                // pin 2 for  laser sensor detection pin
#define ACTION 8                // pin 8 for action to do someting

#define vibration_Sensor 6
#define LED 7

#define RX 10                   // Connect to the TX pin of the HC-12
#define TX 11                   // Connect to the RX pin of the HC-12

#define call_owner1  "ATD+ +8801857715545;"            // put owner phone number
#define sms_owner1 "AT+CMGS=\"+8801857715545\"\r"
#define call_owner2  "ATD+ +8801765014450;"            // put owner phone number
#define sms_owner2 "AT+CMGS=\"+8801765014450\"\r"

#define MOSFET 49
#define buzzer 48

#define PIRpin 43                 // PIR motion sensor pin

#define digitalPin_mq2 47         // mq2 smoke sensor pin 

#define Sone 30                   // flame sensor pins
#define Stwo 31
#define Sthree 32
#define Sfour 33
#define Sfive 34

int Analog_Input_mq2 = A0;        // Analog pin for MQ2 sensor (optional for analog readings)
int lpg, co, smoke;

MQ2 mq2(Analog_Input_mq2);        // MQ2 sensor on analog pin A0


DS3231  rtc(SDA, SCL);            // Init the DS3231 using the hardware interface
bool sensorValidity = true;

SoftwareSerial mySerial(RX, TX);  // init software serial for hc12 communication
SoftwareSerial SIM900A(12,13);    // init software serial for gsm900

String present_time = "";         // RTC variables
String incoming_call_string;

int present_condition = 0;        // vibration variables
int previous_condition = 0;

TMRpcm tmrpcm;                    // needed for sd card module 

int toggle = 1;
bool ifCallMade = false;

void setup(){
  pinMode(MOSFET,OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(MOSFET, HIGH);
  digitalWrite(buzzer,LOW);

  mySerial.begin(9600);         // initializing hc12 software serial as well
  Serial.begin(9600);           // initially was 115200 
  SIM900A.begin(9600);          // GSM Module Baud rate – communication speed

  Serial.println("Initializing...");
  delay(1000);

  SIM900A.println("AT");        //Handshaking with SIM900
  updateSerial();
  SIM900A.println("AT+CSQ");    //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  SIM900A.println("AT+CCID");   //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  SIM900A.println("AT+CREG?");  //Check whether it has registered in the network
  updateSerial();
  SIM900A.println("AT+CLIP=1");
  updateSerial();

  //smoke setup
  pinMode(digitalPin_mq2, INPUT); // Set the digital pin as input
  mq2.begin();                  // Initialize the MQ2 sensor

  //motion setup
  pinMode(PIRpin, INPUT);
  //Serial.begin(9600);           // Start the Serial Monitor


  //flame setup
  pinMode(Sone, INPUT);
  pinMode(Stwo, INPUT);
  pinMode(Sthree, INPUT);
  pinMode(Sfour, INPUT);
  pinMode(Sfive, INPUT);
  
  //laser setup
  pinMode(DETECT, INPUT);       //define detect input pin
  pinMode(ACTION, OUTPUT);      //define ACTION output pin

  //rtc setup
  rtc.begin();
  
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(TUESDAY);          // Set Day-of-Week to SUNDAY
  rtc.setTime(5, 57, 0);        // Set the time to 12:00:00 (24hr format)
  rtc.setDate(11, 3, 2022);     // Set the date to January 1st, 2014

  //vibration setup
  pinMode(vibration_Sensor, INPUT);
  pinMode(LED, OUTPUT);

}


void timeCheck();
void vibrationCheck();
void laserCheck();
void numberCheck();
void smokeCheck();
void motionCheck();
void flameCheck();

void rtcUpdate();
void hc12_Signal();
void playAudio();
void buzzingAndBlink();

void loop(){
  //rtcUpdate();      // update from RTC
  timeCheck();        // checking the time for sensor validity 
  vibrationCheck();   // checking if vibration detected, delay = 7 sec
  //laserCheck();     // checking if laser interference is interfered

  smokeCheck();       // delay = 0.5 sec
  motionCheck();
  flameCheck();
  
  numberCheck();      // check if owner called to turn sensor off/on

  //  if (SIM900A.available()>0)
  //  Serial.write(SIM900A.read());
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


void flameCheck(){
  bool value1 = digitalRead(Sone);
  bool value2 = digitalRead(Stwo);
  bool value3 = digitalRead(Sthree);
  bool value4 = digitalRead(Sfour);
  bool value5 = digitalRead(Sfive);

  //Serial.println(value5);
  if (value1 == 1 || value2 == 1 || value3 == 1 || value4 ==1 || value5 == 1) {

    Serial.println("Flame detected");

    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    mySerial.write("FLAME DETECTED");         // sending alert signal to receiving hc12
    
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
  }
  
}

void motionCheck(){
  int sensorValue = digitalRead(PIRpin);

  if (sensorValue == HIGH) {
    Serial.println("Motion detected!");
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    mySerial.write("MOTION DETECTED");         // sending alert signal to receiving hc12

  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
    Serial.println("No motion.");
  }

  //delay(100);  // Check every 100ms
}

void smokeCheck(){
  int gasDetected = digitalRead(digitalPin_mq2);

  lpg = mq2.readLPG();    // Read LPG concentration
  co = mq2.readCO();      // Read CO concentration
  smoke = mq2.readSmoke();// Read Smoke concentration

  if (gasDetected == HIGH) {
    Serial.println("Gas detected! Checking levels...");
    
    // Print the sensor readings in the Serial Monitor
    Serial.print("LPG: ");
    Serial.print(lpg);
    Serial.print(" ppm | CO: ");
    Serial.print(co);
    Serial.print(" ppm | Smoke: ");
    Serial.print((smoke * 100) / 1000000);
    Serial.println(" %");


    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    mySerial.write("SMOKE DETECTED");         // sending alert signal to receiving hc12

    
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
    Serial.println("No gas detected.");
  }
  
  delay(100);  // Delay for 1 second

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
  {                               // only functional when sensorValidity == true
    //vibration input
    previous_condition = present_condition;
    present_condition = digitalRead(A5);            // Reading digital data from the A5 Pin of the Arduino.
    if (previous_condition != present_condition) {
      digitalWrite(buzzer, HIGH);
      digitalWrite(LED, HIGH);
      mySerial.write("VIBRATION DETECTED");         // sending alert signal to receiving hc12
      Serial.println("VIBRATION DETECTED");
      Makecall(1);                                   // calling owner to alert delay = 6 sec

    }else{
      digitalWrite(buzzer, LOW);
      digitalWrite(LED, LOW);
    }
  }

  delay(100);
}

// Function to check if laser detection is interfered
void laserCheck(){
  int detected = digitalRead(DETECT);       // read Laser sensor 
  if(detected == HIGH){
    Serial.println("Detected!");
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
  }else{
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    Serial.println("No laser");
    mySerial.write("LASER COMPROMISED");    // sending alert signal to receiving hc12
    Makecall(1);                            // calling owner to alert delay = 6 sec
  }
  delay(200);
}



// Verify owner phone number
void numberCheck(){
  if (SIM900A.available()>0){
    incoming_call_string = SIM900A.readString();
    Serial.println("available");
  }

  if(ifCallMade == false && (incoming_call_string.indexOf("+88018577155") > -1  ) ){    // Check if the string we read had any substring containing the owner number
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
  SIM900A.println(call_owner1);
  Serial.write ("call made  to owner 1");
  
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
