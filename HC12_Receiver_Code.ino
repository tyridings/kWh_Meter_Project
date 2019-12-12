// Ty Ridings & Erik Allder
// MEGN 200 Final Project 
// HC12 RECEIVER CODE

/* This code is for the dedicated central hub that receives the data from 
 * the Smart kWh Plug via HC12 transceivers. The wiring is relatively easy, 
 * an HC12 Transceiver wired to an Arduino Uno via:
 * VCC -> 5V
 * GND -> GND
 * TXD -> D10
 * RXD -> D11
 */

#include <SoftwareSerial.h>       // Serial library needed for RX/TX communication
SoftwareSerial HC12(10, 11);      // HC-12 TX Pin, HC-12 RX Pin

char incomingByte;                // Defining variable for incoming Bytes from other HC12
String readBuffer = "";           // Empty String for the incoming transmission to occupy

void setup() {
  Serial.begin(9600);             // Open serial port to computer
  HC12.begin(9600);               // Open serial port to HC12
}

void loop() {
  int ampRead = 0;
  readBuffer = "";
  boolean start = false;
  
  // Reads the incoming amp reading
  while (HC12.available()) {             // If HC-12 has data
    incomingByte = HC12.read();          // Store each icoming byte from HC-12
    delay(5);
    
    // Reads the data between the start "<" and end marker ">"
    if (start == true) {
      if (incomingByte != '>') {
        readBuffer += String(incomingByte);    // Add each byte to ReadBuffer string variable
      }
      else {
        start = false;         // Changes to false once full message received at ">" value
      }
    }
    
    // Checks whether the received message statrs with the start marker "<"
    else if ( incomingByte == '<') {
      start = true;            // If true start reading the message
    }
  }

  Serial.println(readBuffer); // Prints Voltage and Amperage readings to the Serial Monitor
  delay(340);                 // Calibrated delay to not get overflow or underflow of data printing
}
