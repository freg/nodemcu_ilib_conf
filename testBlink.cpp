/*This is where you write the code you want to run*/
#include "Arduino.h"
#include "testBlink.h"
const int ledPin =  LED_BUILTIN;// the number of the LED pin
static int mypin = LED_BUILTIN;
void initblinkLED(int pin)
{
  if (pin==-1)
    pin = ledPin;
  pinMode(pin, OUTPUT);
  mypin = pin ;
}
void blinkLED(int delayTime){
  pinMode(mypin, OUTPUT);
  digitalWrite(mypin, HIGH);
  delay(delayTime);
  digitalWrite(mypin, LOW);
  delay(delayTime);  
}

