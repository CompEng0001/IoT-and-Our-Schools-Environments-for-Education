/*************************************************************************************************************************************************
    Sketch developed by User: CompEng0001
    Creation Date: 04/02/2020
    Verison: 1.2
    Hardware: Arduino MKR 1010 uC, Dust sensor
    Documentation: https://github.com/CompEng0001/IoT-and-Our-Schools-Enivronments-for-Education
    Licence: Attribution-NonCommercial-ShareAlike 4.0 International [CC BY-NC-SA 4.0] (Where licences for hardware and software do not conflict)
**************************************************************************************************************************************************/

/* Required Libaries */
#include <math.h>            // for some math functions

/* Dust Sensor variables */
int pin = 5; // Digital Pin 0 of Ardunio
unsigned long duration;
unsigned long starttime;
float concentration = 0;

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT); // Set Dust Sensor pin to INPUT
  setupConnectivity();
}

// this runs forever, or until the microController is turned off.
void loop()
{
  getDustConcentration(); // this takes 30 seconds
  displayData();
  delay(30000);
}

/**
 * getDustConcentration() retrieves the the approx matter in the air as it passes through the laser.
 * The takes 30 seconds, no way around it. 
*/
void getDustConcentration()
{
  bool data = false;
  unsigned long lowpulseoccupancy = 0;
  unsigned long sampletime_ms = 30000; // sample 30s
  
  while (!data)
  {
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
    if ((millis() - starttime) > sampletime_ms) //if elapsed time is > sampel time == 30s
    {
      float ratio = lowpulseoccupancy / (sampletime_ms * 10.0);                             // Integer percentage 0=>100
      concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
      lowpulseoccupancy = 0;
      starttime = millis();
      data = true;
    }
  }
}

/**
  Display all sensor data on to serial monitor for debugging purposes
*/
void displayData()
{
  // Print dust concentration
  Serial.print("concentration = ");
  Serial.print(concentration);
  Serial.println(" pcs/0.01cf"); // Particles/per 0.1cubic feet
  Serial.println();              // New line escape character

}

/**
  connectivity function sets up the connection to BMP280 and WiFi
*/
void setupConnectivity()
{
  Serial.begin(115200); // set the baud rate, the number of bits per second
  delay(1000);          // give serial chance to settle
}