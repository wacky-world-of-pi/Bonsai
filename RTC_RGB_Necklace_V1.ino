//
// Maurice Ribble
// 4-17-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code tests the DS1307 Real Time clock on the Arduino board.
// The ds1307 works in binary coded decimal or BCD.  You can look up
// bcd in google if you aren't familior with it.  There can output
// a square wave, but I don't expose that in this code.  See the
// ds1307 for it's full capabilities.

#include "Wire.h"
#define DS1307_I2C_ADDRESS 0x68

//all the RGB LED example stuff in here comes from "strandtestsingleled" sketch
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 6, NEO_GRB + NEO_KHZ800);

/// Residual from testing in the cuckoo clock with another I2C device.///
/// You've got to give them each an address or they don't work right  ///
// #include <Adafruit_PWMServoDriver.h>
// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


int pin = 13;
// for interrupt state
volatile int state = LOW;
int counter = 0; //for trackng loops of the interrupt 
int val = 0;

void setup()
{
  //some RGB LED stuff
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'


  // set the led pin to be an output. using led on the pro mini to troubleshoot things
  pinMode(pin, OUTPUT);
  // for setting the time with an interrupt button on pin #2 (interrupt 0) //
  attachInterrupt(0, blink, RISING);
  // end of stuff for setting time //
  //to communicate with the board

  Serial.begin(9600);

  // For clock and serial out //
  Wire.begin();


  // Change these values to what you want to set your clock to.
  // You probably only want to set your clock once and then remove
  // the setDateDs1307 call.


}

void loop()
{

  Serial.print("counter is ");
  Serial.println(counter, DEC);
  Serial.print("state is ");
  Serial.println(state);

  if (state == HIGH){
    setTime();
    state = LOW;
  } 

  //end of that business

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print("  ");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print("  Day_of_week:");
  Serial.println(dayOfWeek, DEC);  
  delay(1000);


  //colors based on time...
  //  if (hour >= 20 && hour <= 23) {   //if less than or equal to a certain number of hours, do the following things...
  if (second >= 5 && second <= 7){
    //strip.setPixelColor format is (pixel#, red, green, blue);
    strip.setPixelColor(0,0,0, 100);  
    strip.show(); // Initialize all pixels to 'off'
  }
  else if (second >= 15 && second <= 17){
    //strip.setPixelColor format is (pixel#, red, green, blue);
    strip.setPixelColor(0,0,0,100);  
    strip.show(); // Initialize all pixels to 'off'
  }
  else if (second >= 25 && second <= 27){
    //strip.setPixelColor format is (pixel#, red, green, blue);
    strip.setPixelColor(0,0,0,100);  
    strip.show(); // Initialize all pixels to 'off'
  }
  else if (second >= 35 && second <= 37){
    //strip.setPixelColor format is (pixel#, red, green, blue);
    strip.setPixelColor(0,0,0,100);  
    strip.show(); // Initialize all pixels to 'off'
  }
  else if (second >= 45 && second <= 47){
    //strip.setPixelColor format is (pixel#, red, green, blue);
    strip.setPixelColor(0,0,0,100);  
    strip.show(); // Initialize all pixels to 'off'
  }
          else if (second >= 55 && second <= 57){
      //strip.setPixelColor format is (pixel#, red, green, blue);
      strip.setPixelColor(0,0,0,100);  
      strip.show(); // Initialize all pixels to 'off'
    }
  else {
    strip.setPixelColor(0,0,0,0);  
    strip.show(); // Initialize all pixels to 'off'
  }
  //  }
}




// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

// Stops the DS1307, but it has the side effect of setting seconds to 0
// Probably only want to use this for testing
/*void stopDs1307()
 {
 Wire.beginTransmission(DS1307_I2C_ADDRESS);
 Wire.write(0);
 Wire.write(0x80);
 Wire.endTransmission();
 }*/

// 1) Sets the date and time on the ds1307
// 2) Starts the clock
// 3) Sets hour mode to 24 hour clock
// Assumes you're passing in valid numbers
void setDateDs1307(byte second,        // 0-59
byte minute,        // 0-59
byte hour,          // 1-23
byte dayOfWeek,     // 1-7
byte dayOfMonth,    // 1-28/29/30/31
byte month,         // 1-12
byte year)          // 0-99
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

// Gets the date and time from the ds1307
void getDateDs1307(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

  // A few of these need masks because certain bits are control bits
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

void blink()
{
  state = HIGH;
  counter = 1;
}

void setTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  second = 00;
  minute = 22;
  hour = 22;
  dayOfWeek = 6;
  dayOfMonth = 17;
  month = 8;
  year = 14;
  setDateDs1307(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
  // end of clock and serial out // 
}
//some RGB LED supporting stuff


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}









