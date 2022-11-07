
// Arduino Mega:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//

#include <DS3231.h>
#include <stdio.h>
#define ON 1
#define OFF 0


// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

int vibration_Sensor = 6;
int LED = 13;

//vibration variables
int present_condition = 0;
int previous_condition = 0;

void setup(){
  Serial.begin(115200);
  rtc.begin();
  
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(TUESDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(10, 25, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(11, 3, 2022);   // Set the date to January 1st, 2014

  //vibration setup
  pinMode(vibration_Sensor, INPUT);
  pinMode(LED, OUTPUT);
}

void led_blink(void);
void vibrationCheck();
void rtcUpdate();

void loop(){
  
  rtcUpdate();
  vibrationCheck(); 
  // Wait one second before repeating :)
  delay (1000);   //this delay needed for rtc, otherwise code cant be uploaded
}


void rtcUpdate(){
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  // Send time
  Serial.println(rtc.getTimeStr());
}

void vibrationCheck(){
  //vibration input
  previous_condition = present_condition;
  present_condition = digitalRead(A5); // Reading digital data from the A5 Pin of the Arduino.

  if (previous_condition != present_condition) {
    led_blink();
  }else{
    digitalWrite(LED, OFF);
  }
}


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
