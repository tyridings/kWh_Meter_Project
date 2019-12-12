// Ty Ridings & Erik Allder 
// MEGN 200 Final PROJECT
// HC12 TRANSMITTER CODE

/* This code is dedicated to the Smart Plug device that uses a 
 * current transformer, arduino nano, OLED display, hc12 transceiver, and various
 * resistors and a capacitor. For the full wiring setup, visit my github:
 * https://github.com/tyridings
 * 
 * The functionality premise: take readings from CT, process data and generate
 * amp/watt readings, send the data to central hub via hc12, print info on OLED display
 */

#include "EmonLib.h"                //Energy Monitor Library 
#include <SoftwareSerial.h>         // Used for the TX/RX Transceiver coms 
SoftwareSerial HC12(10, 11);        // HC-12 TX Pin, HC-12 RX Pin
EnergyMonitor emon1;                // Name for Energy Device

/*--------Libraries, Definitions, and other stuff Needed for the OLED----------*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
byte count;
byte sensorArray[128];
byte drawHeight;
char filled = 'F';           //decide either filled or dot display (D=dot, any else filled)
char drawDirection = 'R';    //decide drawing direction, from right or from left (L=from left to right, any else from right to left)
char slope = 'W';            //slope colour of filled mode white or black slope (W=white, any else black. Well, white is blue in this dispay but you get the point)
/*--------------------------------------*/

int counter = 0;             // Counter that tracks the number of seconds for Cool Terms Data

void setup()
{
  HC12.begin(9600);          // Serial port to HC12 Baud Rate
  Serial.begin(9600);        // Serial Monitor Baud Rate
  emon1.current(1, 111.1);   // Current: input pin, calibration.
  
/*---OLED SETUP---*/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  for (count = 0; count <= 128; count++)      //zero all elements
  {
    sensorArray[count] = 0;
  }
/*----------------*/
}


void loop()
{
double Irms = emon1.calcIrms(1480);       // Calculate Irms only
float amps = (0.2671 * Irms) + 0.3971;    // Function for calibrating the Amps 
String ampString = String(amps);          // String to send Amps data via HC12
float watts = amps * 100;                 // Watt approximation from amps
int graphRead = watts;                    // Int value of Watts needed for OLED graph
String wattString = String(watts);        // String to send Watts data via HC12

  if (amps < 0.6){
    watts = 0;                     // Sets Watts = 0 when no current running through plug
    Serial.print(counter);         // Seconds Counter
    Serial.print(" Seconds ");
    Serial.print(watts);           // Apparent power
    Serial.print(" Watts ");
    Serial.print(amps);            // Amperage Reading
    Serial.println(" Amps");
  }
  else {
    Serial.print(counter);         // Seconds Counter
    Serial.print(" Seconds ");
    Serial.print(watts);           // Apparent power
    Serial.print(" Watts ");
    Serial.print(amps);            // Amperage Reading
    Serial.println(" Amps");
  }

counter += 1;                      // Add one second to the counter ever iteration

/*--This sends the string data to the central hub via the HC12 Transceiver:--*/
HC12.print("<" + ampString + " amps " + wattString + " watts " + ">");
//delay(100);

/*------LOOP OF THE OLED FUNCTION PRINTING THE GRAPH AND WATT VALUES-----*/
  drawHeight = map(graphRead, 8, 1023, 0, 32 );
  sensorArray[0] = drawHeight;
  for (count = 1; count <= 127; count++ )
  {
    if (filled == 'D' || filled == 'd')
    {
      if (drawDirection == 'L' || drawDirection == 'l')
      {
        display.drawPixel(count, 32 - sensorArray[count - 1], WHITE);
      }
      else //else, draw dots from right to left
      {
        display.drawPixel(127 - count, 32 - sensorArray[count - 1], WHITE);
      }
    }
    else
    {
      if (drawDirection == 'L' || drawDirection == 'l')
      {
        if (slope == 'W' || slope == 'w')
        {
          display.drawLine(count, 32, count, 32 - sensorArray[count - 1], WHITE);
        }
        else
        {
          display.drawLine(count, 1, count, 32 - sensorArray[count - 1], WHITE);
        }
      }
      else
      {
        if (slope == 'W' || slope == 'w')
        {
          display.drawLine(127 - count, 32, 127 - count, 32 - sensorArray[count - 1], WHITE);
        }
        else
        {
          display.drawLine(127 - count, 1, 127 - count, 32 - sensorArray[count - 1], WHITE);
        }
      }
    }
  }
  drawAxises();
  display.display();        //needed before anything is displayed
  display.clearDisplay();   //clear before new drawing

  for (count = 127; count >= 2; count--) // count down from 160 to 2
  {
    sensorArray[count - 1] = sensorArray[count - 2];
  }
}

// FUNCTION SETUPS FOR THE OLED GRAPHING FEATURE
//separate to keep stuff neater. This controls ONLY drawing background!
void drawAxises()  
{
  display.setCursor(50, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(drawHeight);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Watts");

//this draws the left- and right-side vertical axes
  display.drawLine(0, 8, 0, 32, WHITE);
  display.drawLine(127, 8, 127, 32, WHITE);

//this draws the tick marks on the right-side vertical axis
  for (count = 8; count < 40; count += 10)//this sets the height limit for the tick marks on the scrolling graph
  {
    display.drawLine(127, count, 122, count, WHITE);
    display.drawLine(0, count, 5, count, WHITE);
  }
//this draws the tick marks on the left-side vertical axis
  for (count = 10; count < 127; count += 10)
  {
    display.drawPixel(count, 8 , WHITE);
    display.drawPixel(count, 31 , WHITE);
  }
}
