#include <SoftwareSerial.h>

SoftwareSerial SIM900A(7,8);

void setup()
{
SIM900A.begin(9600); // GSM Module Baud rate – communication speed
Serial.begin(9600); // Baud rate of Serial Monitor in the IDE app
Serial.println ("Text Messege Module Ready & Verified");
delay(100);
Serial.println ("Type s to send message or r to receive message");
}

void loop()
{
if (Serial.available()>0)
switch(Serial.read())
{
case 's':
SendMessage();
break;
case 'r':
RecieveMessage();
break;
case 'a':
Makecall();
break;
case 'b':
Receivingcall();
break;
}

if (SIM900A.available()>0)
Serial.write(SIM900A.read());
}

void SendMessage()
{
Serial.println ("Sending Message please wait….");
SIM900A.println("AT+CMGF=1"); //Text Mode initialisation
delay(1000);
Serial.println ("Set SMS Number");
SIM900A.println("AT+CMGS=\"+8801857715545\"\r"); // Receiver’s Mobile Number
delay(1000);
Serial.println ("Set SMS Content");
SIM900A.println("Please subscribe friendtechbd youtube channel ");// Messsage content
delay(100);
Serial.println ("Done");
SIM900A.println((char)26);// delay(1000);
Serial.println ("Message sent succesfully");
}

void RecieveMessage()
{
Serial.println ("Receiving Messeges");
delay (1000);
SIM900A.println("AT+CNMI=2,2,0,0,0"); // Eeceiving Mode Enabled
delay(1000);
Serial.write ("Messege Received Sucessfully");
}
void Makecall()
{
Serial.println ("Makeing calling");
delay (1000);
SIM900A.println("ATD+ +8801857715545;");
delay(1000);
Serial.write ("make call");
//SIM900A.println("ATH"); //For call hang up

}

void Receivingcall()
{
Serial.println ("Receiving call");
delay (1000);
SIM900A.println("ATA"); // Eeceiving Mode Enabled
delay(1000);
Serial.write ("receive call Sucessfully");
}
