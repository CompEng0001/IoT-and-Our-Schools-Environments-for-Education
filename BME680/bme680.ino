/*************************************************************************************************************************************************
    Sketch developed by User: CompEng0001
    Creation Date: 04/02/2020
    Verison: 1.2
    Hardware: Arduino MKR 1010 uC, BME680
    Documentation: https://github.com/CompEng0001/IoT-and-Our-Schools-Enivronments-for-Education
    Licence: Attribution-NonCommercial-ShareAlike 4.0 International [CC BY-NC-SA 4.0] (Where licences for hardware and software do not conflict)
**************************************************************************************************************************************************/

/* Required Libaries */
#include <math.h>            // for some math functions
#include <seeed_bme680.h>    // for BME680 sensor

Seeed_BME680 bme680(IIC_ADDR); //IIC address remember 0x76
/* BME680 Sensor variables */
float temp, hum, bar, voc, IAQ_Value;
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define IIC_ADDR uint8_t(0x76)

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT); // Set Dust Sensor pin to INPUT
  setupConnectivity();
}

// this runs forever, or until the microController is turned off.
void loop()
{
  getBMEValues(); // get the Temperature, humidity, pressure and VOC (IAQ Index value)
  displayData();
  delay(2000);
}

/**
  getBMEValues gets the temp, humidity, pressure and VOC
  VOC is Calculated to be returned as IAQ_Value
*/
void getBMEValues()
{
  if (bme680.read_sensor_data()) {
    //debugging only
    Serial.println("Failed to perform reading :(");
    return;
  }
  temp = bme680.sensor_result_value.temperature;         // Get the Temperature reading from the BMP280
  bar = bme680.sensor_result_value.pressure / 100.0;   // Get the Pressure reading from the BMP280

  hum = bme680.sensor_result_value.humidity;
  voc += bme680.sensor_result_value.gas;

  // Calculate the IAQ Value
  int hum_ref = 40, gas_ref = 0, gasLL = 5000, gasUL = 50000;
  float humscore=0.00, gasscore=0.00;

  // Get humidity score between 0.00 and 100.00 
  if(hum >= 38 && hum <=42) { humscore = 0.25 *100; }
  else if (hum < 38) { humscore = ((0.25/hum_ref)*hum)*100; }
  else { humscore = (((-0.25/(100-hum_ref))*hum)+0.416666)*100; }

  // Get VOC score between 0.00 and 100.00
  if(voc >= gasUL) { gas_ref = gasUL; } 
  else if (voc <= gasLL) { gas_ref = gasLL; }
  else { gas_ref = (int)voc;}
  gasscore = (0.75/(gasUL-gasLL)*gas_ref - (gasLL*(0.75/(gasUL-gasLL))))*100;
  
  // Get IAQ Value between 0.00 and +300.00%
  IAQ_Value = (100-(humscore+gasscore))*5;
}


/**
  Display all sensor data on to serial monitor for debugging purposes
*/
void displayData()
{
  //Print BME280 data
  Serial.print("The pressure (kpa) is: ");
  Serial.println(bar);
  Serial.print("The Temperature (C) is: ");
  Serial.println(temp);
  Serial.print("The Humidity(%)is: ");
  Serial.println(hum);
  Serial.print("The VOC (khoms) is: ");
  Serial.println(voc);
  Serial.print("The IAQ score (%) is: ");
  Serial.println(IAQ_Value);
  Serial.println();
}

/**
  connectivity function sets up the connection to BMP280 and WiFi
*/
void setupConnectivity()
{
  Serial.begin(115200); // set the baud rate, the number of bits per second
  delay(1000);          // give serial chance to settlE

  // Initialized false by default
  while (!bme680.init())
  {
    Serial.println("bme680 init failed ! can't find device!");
    delay(10000);
  }

  Serial.println("");  Serial.println("BME680 is connected");

}
