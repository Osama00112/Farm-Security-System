#include <SoftwareSerial.h>
#include <DS3231.h>
#include <SD.h>  
#include <TMRpcm.h>                  
#include <SPI.h>
#include <stdio.h>
#include <MQ2.h>

#define ON 1
#define OFF 0

#define DETECT 2                // pin 2 for laser sensor detection pin
#define ACTION 8                // pin 8 for action to do something

#define vibration_Sensor A5
#define LED 7

#define RX 10                   // Connect to the TX pin of the HC-12
#define TX 11                   // Connect to the RX pin of the HC-12

#define call_owner1  "ATD+ +8801857715545;"            
#define sms_owner1 "AT+CMGS=\"+8801857715545\"\r"
#define call_owner2  "ATD+ +8801765014450;"            
#define sms_owner2 "AT+CMGS=\"+8801765014450\"\r"

#define MOSFET 49
#define buzzer 48

#define PIRpin 43               // PIR motion sensor pin
#define digitalPin_mq2 47       // mq2 smoke sensor pin 

#define Sone 30                 // flame sensor pins
#define Stwo 31
#define Sthree 32
#define Sfour 33
#define Sfive 34

int Analog_Input_mq2 = A0;      // Analog pin for MQ2 sensor
int lpg, co, smoke;

MQ2 mq2(Analog_Input_mq2);      // MQ2 sensor on analog pin A0

DS3231 rtc(SDA, SCL);           // Init the DS3231 using the hardware interface
bool sensorsEnabled = true;      // Global flag to track sensor state
bool alarmTriggered = false;     // Flag to prevent multiple calls for the same event

SoftwareSerial mySerial(RX, TX);  // init software serial for hc12 communication
SoftwareSerial SIM900A(12,13);    // init software serial for gsm900

String present_time = "";         // RTC variables
String incoming_call_string;

int present_condition = 0;        // vibration variables
int previous_condition = 0;

TMRpcm tmrpcm;                    // needed for sd card module 

bool ifCallMade = false;

void setup() {
    pinMode(MOSFET, OUTPUT);
    pinMode(buzzer, OUTPUT);
    digitalWrite(MOSFET, HIGH);    // Start with sensors enabled
    digitalWrite(buzzer, LOW);
    sensorsEnabled = true;

    mySerial.begin(9600);         // initializing hc12 software serial
    Serial.begin(9600);           
    SIM900A.begin(9600);          // GSM Module Baud rate

    Serial.println("Initializing...");
    delay(1000);

    // Initialize GSM module
    SIM900A.println("AT");        // Handshaking with SIM900
    updateSerial();
    SIM900A.println("AT+CSQ");    // Signal quality test
    updateSerial();
    SIM900A.println("AT+CCID");   // Read SIM information
    updateSerial();
    SIM900A.println("AT+CREG?");  // Check network registration
    updateSerial();
    SIM900A.println("AT+CLIP=1"); // Enable caller ID
    updateSerial();

    // Sensor pins setup
    pinMode(digitalPin_mq2, INPUT);
    mq2.begin();
    pinMode(PIRpin, INPUT);
    pinMode(Sone, INPUT);
    pinMode(Stwo, INPUT);
    pinMode(Sthree, INPUT);
    pinMode(Sfour, INPUT);
    pinMode(Sfive, INPUT);
    pinMode(DETECT, INPUT);
    pinMode(ACTION, OUTPUT);
    pinMode(vibration_Sensor, INPUT);
    pinMode(LED, OUTPUT);

    // Initialize RTC
    rtc.begin();
    // Uncomment to set the date and time
    // rtc.setDOW(TUESDAY);
    // rtc.setTime(5, 57, 0);
    // rtc.setDate(11, 3, 2022);
}

void loop() {
    rtcUpdate();
    timeCheck();
    
    if (sensorsEnabled) {
        vibrationCheck();
        laserCheck();
        smokeCheck();
        motionCheck();
        flameCheck();
    }
    
    numberCheck();      // Always check for owner calls
    delay(500);
}

void rtcUpdate() {
    present_time = rtc.getTimeStr();
    Serial.println(rtc.getTimeStr());
}

void timeCheck() {
    // 7 AM - Turn off sensors
    if ((present_time[0] == '0' && present_time[1] == '7' && present_time[2] == ':') || 
        (present_time[0] == '7' && present_time[1] == ':')) {
        sensorsEnabled = false;
        digitalWrite(MOSFET, LOW);
        Serial.println("Its 7 AM. Sensors turned OFF");
    }
    
    // 7 PM - Turn on sensors
    if (present_time[0] == '1' && present_time[1] == '9' && present_time[2] == ':') {
        sensorsEnabled = true;
        digitalWrite(MOSFET, HIGH);
        Serial.println("Its 7 PM. Sensors turned ON");
    }
}

void vibrationCheck() {
    previous_condition = present_condition;
    present_condition = digitalRead(vibration_Sensor);
    
    if (previous_condition != present_condition && !alarmTriggered) {
        triggerAlarm("VIBRATION DETECTED");
    } else if (previous_condition == present_condition) {
        resetAlarm();
    }
    delay(100);
}

void laserCheck() {
    int detected = digitalRead(DETECT);
    if (detected == HIGH) {
        resetAlarm();
    } else if (!alarmTriggered) {
        triggerAlarm("LASER COMPROMISED");
    }
    delay(200);
}

void smokeCheck() {
    int gasDetected = digitalRead(digitalPin_mq2);
    
    if (gasDetected == HIGH && !alarmTriggered) {
        lpg = mq2.readLPG();
        co = mq2.readCO();
        smoke = mq2.readSmoke();
        
        Serial.print("LPG: ");
        Serial.print(lpg);
        Serial.print(" ppm | CO: ");
        Serial.print(co);
        Serial.print(" ppm | Smoke: ");
        Serial.print((smoke * 100) / 1000000);
        Serial.println(" %");
        
        triggerAlarm("SMOKE DETECTED");
    } else if (gasDetected == LOW) {
        resetAlarm();
    }
    delay(100);
}

void motionCheck() {
    int sensorValue = digitalRead(PIRpin);
    
    if (sensorValue == HIGH && !alarmTriggered) {
        triggerAlarm("MOTION DETECTED");
    } else if (sensorValue == LOW) {
        resetAlarm();
    }
}

void flameCheck() {
    bool value1 = digitalRead(Sone);
    bool value2 = digitalRead(Stwo);
    bool value3 = digitalRead(Sthree);
    bool value4 = digitalRead(Sfour);
    bool value5 = digitalRead(Sfive);
    
    if ((value1 == 1 || value2 == 1 || value3 == 1 || value4 == 1 || value5 == 1) && !alarmTriggered) {
        triggerAlarm("FLAME DETECTED");
    } else if (value1 == 0 && value2 == 0 && value3 == 0 && value4 == 0 && value5 == 0) {
        resetAlarm();
    }
}

void triggerAlarm(const char* message) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    mySerial.write(message);
    Serial.println(message);
    alarmTriggered = true;
    Makecall(1);
}

void resetAlarm() {
    digitalWrite(buzzer, LOW);
    digitalWrite(LED, LOW);
    alarmTriggered = false;
}

void numberCheck() {
    if (SIM900A.available() > 0) {
        incoming_call_string = SIM900A.readString();
        Serial.println("Call incoming");
    }

    if (!ifCallMade && (incoming_call_string.indexOf("+88018577155") > -1 || 
        incoming_call_string.indexOf("+8801765014450") > -1)) {
        
        sensorsEnabled = !sensorsEnabled;
        digitalWrite(MOSFET, sensorsEnabled ? HIGH : LOW);
        
        // Reset all states
        resetAlarm();
        Serial.println(sensorsEnabled ? "Sensors ENABLED" : "Sensors DISABLED");
        
        incoming_call_string = "";
        delay(3000);
    } else if (ifCallMade) {
        Serial.println("Call was from alarm system");
        ifCallMade = false;
        incoming_call_string = "";
    }
}

void SendMessage(String msg) {
    Serial.println("Sending Message...");
    SIM900A.println("AT+CMGF=1");    // Text Mode initialization
    delay(1000);
    
    SIM900A.println(sms_owner1);
    SIM900A.println(sms_owner2);
    delay(1000);
    
    SIM900A.println(msg);
    delay(100);
    
    SIM900A.println((char)26);
    Serial.println("Message sent successfully");
}

void Makecall(int num) {
    Serial.println("Calling owner");
    delay(500);
    SIM900A.println(call_owner1);
    Serial.println("Call made to owner 1");
    delay(4000);
    ifCallMade = true;
}

void updateSerial() {
    delay(500);
    while (Serial.available()) {
        SIM900A.write(Serial.read());
    }
    while (SIM900A.available()) {
        Serial.write(SIM900A.read());
    }
}
