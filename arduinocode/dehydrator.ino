/*
** Arduino Dehydrator code for Dehydrator with 1602 LCD, temp sensor and rotary encoder
** by
** Hugo van Duijn
** http://www.makerwebshop.com
** LICENSE: GNU General Public License, version 3 (GPL-3.0)

** This example uses F Malpartida's NewLiquidCrystal library. Obtain from:
** https://bitbucket.org/fmalpartida/new-liquidcrystal
** I used version 1.2.1 https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/LiquidCrystal_V1.2.1.zip

** Written for and tested with Arduino 1.0
**
*/


#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>


// defines for 1602 LCD
#define I2C_ADDR    0x3F 
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
                     

// Defines for rotary knob ky-040
volatile boolean TurnDetected;
volatile boolean up;

const int PinCLK=2;                   // Used for generating interrupts using CLK signal
const int PinDT=3;                    // Used for reading DT signal
const int PinSW=4;                    // Used for the push button switch

//more code for 1602 LCD 
void isr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
 if (digitalRead(PinCLK))
   up = !digitalRead(PinDT);
 else
   up = digitalRead(PinDT);
 TurnDetected = true;
}
int diff;
int n = 1;
int start = -1;
LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

//Defines for KY015 DHT11 Temperature and humidity sensor 
int DHpin = 8;
byte dat [5];
byte read_data () {
  byte data;
  for (int i = 0; i < 8; i ++) {
    if (digitalRead (DHpin) == LOW) {
      while (digitalRead (DHpin) == LOW); // wait for 50us
      delayMicroseconds (30); // determine the duration of the high level to determine the data is '0 'or '1'
      if (digitalRead (DHpin) == HIGH)
        data |= (1 << (7-i)); // high front and low in the post
      while (digitalRead (DHpin) == HIGH); // data '1 ', wait for the next one receiver
     }
  }
return data;
}

// code for temp + moisture reading
void start_test () {
  digitalWrite (DHpin, LOW); // bus down, send start signal
  delay (30); // delay greater than 18ms, so DHT11 start signal can be detected
 
  digitalWrite (DHpin, HIGH);
  delayMicroseconds (40); // Wait for DHT11 response
 
  pinMode (DHpin, INPUT);
  while (digitalRead (DHpin) == HIGH);
  delayMicroseconds (80); // DHT11 response, pulled the bus 80us
  if (digitalRead (DHpin) == LOW);
  delayMicroseconds (80); // DHT11 80us after the bus pulled to start sending data
 
  for (int i = 0; i < 4; i ++) // receive temperature and humidity data, the parity bit is not considered
    dat[i] = read_data ();
 
  pinMode (DHpin, OUTPUT);
  digitalWrite (DHpin, HIGH); // send data once after releasing the bus, wait for the host to open the next Start signal
}


void setup()
{
  
  
// Initialise the Arduino data pins for OUTPUT, these are the 2 Solid State Relay Pins

  pinMode(10, OUTPUT);       
  pinMode(11, OUTPUT);      
  
// Set modes for rotaty encoder pins
 pinMode(PinCLK,INPUT);
 pinMode(PinDT,INPUT);  
 pinMode(PinSW,INPUT);
 pinMode (DHpin, OUTPUT);
 attachInterrupt (0,isr,FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
 Serial.begin (9600);
 Serial.println("Start");
 
// lcd setup
  lcd.begin (16,2); 

// Switch on the LCD backlight
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);
lcd.home (); // go home
lcd.setBacklight(HIGH);     // Backlight on
}

void loop()
{

 static long virtualPosition=0;    // without STATIC the rotary encoder does not count correctly!!!

        if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
            start = start * -1;            // if YES, then switch start mode. ( -1 = off, 1 = on )
            while (!digitalRead(PinSW)) {}  // wait til switch is released
            delay(10);                      // debounce
            Serial.println("Power Switch Event");        // Using the word RESET instead of COUNT here to find out a buggy encoder
            }
 if (virtualPosition < 25){ virtualPosition = 25; }  // min temp = 25
 if (virtualPosition > 75){ virtualPosition = 75; }  // max temp = 75
 if (TurnDetected)  {		   // do this only if rotation was detected
   if (up)
     virtualPosition++;
    else
     virtualPosition--;
     TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
   }
   lcd.setCursor (0,1);  

// code when power is on
   if (start == 1){ 
     digitalWrite(10,HIGH);         // Turns ON FAN relays
     lcd.print("on ");                    // Print to LCD  on 
     start_test ();                        // check temp + moist
     delay (400);                        // Delay 400ms, 400ms still makes is possible to adjust temp during on.
       lcd.setCursor (0,0);           
       lcd.print("t:");                   // print info to lcd
       lcd.print (dat [2], DEC);
       lcd.print ("C("); 
       lcd.print (dat [0], DEC);
       lcd.print ("%) s:"); 
       lcd.print(virtualPosition);    // virtualPosition is the temp set by encoder
       diff = virtualPosition- dat [2];  // check difference between set temp and measured temp
       if (diff > 0){           // diff > 0 means set temp is higher than actual temp so start heating
         lcd.setCursor (8,1);   
         if (n < 20){                     // if  heater was turned off less than 20x400ms ago then don't start heating again but wait a moment and display TIMEOUT
           lcd.print(" TIMEOUT ");
           n=n+1;
         }else{
           digitalWrite(11,HIGH);   // Switch Relay 2 to start heating
           lcd.print(" HEATING ");      
           n=99;
         }
       } else {
         lcd.setCursor (8,1);         // if set temp is not higher than actual temp then switch HEATER Relay  off 
            digitalWrite(11,LOW);  
            lcd.print("         ");
         if (n==99){ n=0;}
         n=n+1;
       }
 } else {   // system is set to off
      digitalWrite(10,LOW);           // Turns OFF both relays and display info to LCD
      digitalWrite(11,LOW);  
       lcd.print("off");
          lcd.setCursor (0,0);
       lcd.print("t:");
       lcd.print (dat [2], DEC);
       lcd.print ("C("); 
       lcd.print (dat [0], DEC);
          lcd.print ("%) s:"); 
       lcd.print(virtualPosition);


 }
}
