/*************************************************************************************************************************************************
    Sketch developed by User: CompEng0001
    Creation Date: 04/02/2020
    Verison: 1.2
    Hardware: Arduino MKR 1010 uC, Gas (MQ5) sensor
    Documentation: https://github.com/CompEng0001/IoT-and-Our-Schools-Enivronments-for-Education
    Licence: Attribution-NonCommercial-ShareAlike 4.0 International [CC BY-NC-SA 4.0] (Where licences for hardware and software do not conflict)
**************************************************************************************************************************************************/

/* Required Libaries */
#include <math.h>            // for some math functions

/* MQ5 Sensor variables */
#define GAS_SENSOR A0 // Analogue pin 0 of Arduino
float gasValue = 0.0;

/* object creation */
WiFiClient client;   // create instance of WiFiClient
Seeed_BME680 bme680(IIC_ADDR); //IIC address remember 0x76

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT); // Set Dust Sensor pin to INPUT
  setupConnectivity();
}

// this runs forever, or until the microController is turned off.
void loop()
{
  getGasData();           // Get the reading from the GAS_SENSOR
  displayData();
  delay(30000); // time to rest
}

/**
  getGasData acquires the gas ratio which indicates what gas is being detected
  TODO - find the relationship between RS/RO for each gas in PPM?
*/
void getGasData()
{
  float sensor_volt;
  float RS_gas; // Get value of RS in a GAS
  float ratio;  // Get ratio RS_GAS/RS_air
  int sensorValue = analogRead(GAS_SENSOR);
  sensor_volt = (float)sensorValue / 1024 * 5.0; /// here we want to get the voltage at the out. 
  RS_gas = (5.0 - sensor_volt) / sensor_volt; // omit *RL
  float RO = RS_gas / 6.5;
  /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
  gasValue = RS_gas / RO; // ratio = RS/R0
}

/**
  Display all sensor data on to serial monitor for debugging purposes
*/
void displayData()
{
    //Print gas value
  Serial.print("Air Quality = ");
  Serial.println(gasValue);
}

/**
  connectivity function sets up the connection to BMP280 and WiFi
*/
void setupConnectivity()
{
  Serial.begin(115200); // set the baud rate, the number of bits per second
  delay(1000);          // give serial chance to settlE

}