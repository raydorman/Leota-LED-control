/*

Halloween 2013 
LED strip for globe ("A Poor Man's Leota") by Ray Dorman

Based off Adafruit example test code for LPD8806-based RGB LED strip

2013-07-16 First commit
2013-07-17 Added input for LOR signal
2013-08-06 Added IF statements to deal with LOR signal
2013-08-08 Changed reset to function checked within light sequences

NOTE: Linking between Input 2 and Ground will cause sequence to start,
removing will stop sequence

*/

#include "LPD8806.h"
#include "SPI.h"

int dataPin = 4;      //green wire!
int clockPin = 3;     //yellow wire!
int LORCue1Pin = 2;   //contact closure 1 from LOR show control
boolean reset = false; //indication that show loop should end
 
LPD8806 strip = LPD8806(32, dataPin, clockPin); 

void setup() 
  {
  strip.begin();                // Start up the LED strip
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.print("Arduino has restarted");   
  Serial.println("");
  void turnoff();
  pinMode(LORCue1Pin, INPUT);    // sets the digital pin as input to read switch
  digitalWrite(LORCue1Pin, HIGH);    //enables built in pull up resistor 
  Serial.print("Enable pin initialized, waiting for LOR");
  Serial.println("");
  strip.show();                // Update the strip, to start they are all 'off'
  }

//void dither(uint32_t c, uint8_t wait);
//void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);

void loop() 
  {
    if (reset == true)
      {
        turnoff();
        reset = false;
      }
    
    while (digitalRead(LORCue1Pin)==LOW && reset == false)
    {  
        Serial.print("Enable received from LOR");
        Serial.println("");
        dither(strip.Color(0,32,0), 200);          // green, slow - to make brighter, increase each rgb param by factor of 2         
        dither(strip.Color(15,0,32), 200);          // violet, slow 
        scanner(0,32,0, 300);                       // green scan, slow  
        dither(strip.Color(15,0,0), 200);           // red, slow 
        dither(strip.Color(15,0,32), 200);          // violet, slow
        scanner(0,32,0, 300);                       // green scan, slow    
        dither(strip.Color(0,32,0), 200);           // green, slow
        dither(strip.Color(15,0,32), 200);          // violet, slow 
        dither(strip.Color(15,0,0), 200);           // red, slow 
        dither(strip.Color(15,0,32), 200);          // violet, slow  
        scanner(0,32,0, 300);                       // green scan, slow   
      }   
  }

boolean LORCue1Check()      //checks the status of LOR Cue 1 pin. if active returns TRUE, otherwise false.
  { 
    if (digitalRead(LORCue1Pin)==HIGH) 
      {
       return true;
      } 
    else 
      {
       return false;
      }
   }
  
void turnoff()                                //No enable, no lights
{                                                                     
  Serial.print("LOR says to turn off, waiting for next enable");   
  Serial.println("");
  for (int i = 0; i < 256; i++)                                     
  {                                                                 
    strip.setPixelColor(i, 0,0,0);                                  
    strip.show();   
  }   
    
}     

void dither(uint32_t c, uint8_t wait) 
  {
   
    int hiBit = 0;                                // Determine highest bit needed to represent pixel index
    int n = strip.numPixels() - 1;
    
    
    for(int bit=1; bit < 0x8000; bit <<= 1) 
      {
        if(n & bit) hiBit = bit;
      }
 
    int bit, reverse;
    for(int i=0; i<(hiBit << 1); i++) 
      {                                               // Reverse the bits in i to create ordered dither:
         reverse = 0;
         for(bit=1; bit <= hiBit; bit <<= 1) 
           {
             reverse <<= 1;
             if(i & bit) reverse |= 1;
            }
      strip.setPixelColor(reverse, c);
      strip.show();
      if (LORCue1Check() == true)
        {
         reset = true;
         return;
        } 
        else 
        {
        delay(wait);
        }
      }
  delay(2000);                                         // Hold image in ms
  }
  
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)       // "Larson scanner" = Cylon/KITT bouncing light effect
 { 
  int i, j, pos, dir;
  pos = 0;
  dir = 1;
  for(i=0; i<((strip.numPixels()-1) * 8); i++)                    // Draw 5 pixels centered on pos.
    {                   
     if(LORCue1Check() == true)    
       { 
         reset = true;
         return;               // return == break for functions
       }      
      
      strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
      strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
      strip.setPixelColor(pos, strip.Color(r, g, b));
      strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
      strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));
      strip.show();
      delay(wait);
  
      for(j=-2; j<= 2; j++) 
      strip.setPixelColor(pos+j, strip.Color(0,0,0));                
      pos += dir;                                           // Bounce off ends of strip
      if(pos < 0) 
      {
       pos = 1;
       dir = -dir;
      }   
      else if(pos >= strip.numPixels()) 
      {
        pos = strip.numPixels() - 2;
        dir = -dir;
      }
    }
 }
