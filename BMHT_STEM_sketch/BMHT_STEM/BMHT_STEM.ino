/***************************************************************************
   Sketch developed by User: CompEng0001
   Creation Date: 21/05/2019
   Verison: 1.0.8
   Hardware: NodeMcu, BME280 
***************************************************************************/

#include <Wire.h>        // To make the NodeMcu pins SDA, SCL
#include <ESP8266WiFi.h> // needed for the NodeMcu
#include "ThingSpeak.h"  // downlaod the library - ThingSpeak
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
WiFiClient client;   // create instance of WiFiClient

unsigned long delayTime; // 
char ssid[] = "Raleigh Conference"; // replace with your wifi ssid and wpa2 key
char pass[] = "Raleigh05"; // Replace with SSID pwd
unsigned long myChannelNumber = 792104;         // ThingSpeak Channel number
const char *myWriteAPIKey = "B1NL1Z3AL4Q8ALD5"; //  Enter your Write API key from ThingSpeak`
float temp, hum, bar;
int audio;

void setup()
{
  // create I2C pins [SDA = GPIO4 (D2), SCL = GPIO0 (D3)], default clock is 100kHz
  Wire.begin(4, 0);

  // Initialize Serial So we can see for debugging, logging purposes
  Serial.begin(9600); // this number is the baud rate or how fast data is transmitted in Bits per Second.
  Serial.println(F("BME280 test"));

  // Checks to make sure the BME280 is wired up correctly
  bool status;
  while (!status)
  {
    status = bme.begin(0x76);
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("BME280 is connected");

  // Creating a connection to the WiFi and ThingSpeak
  Serial.println("Connecting to "); // debugging purposes
  Serial.println(ssid);
  ThingSpeak.begin(client); // Initialize ThingSpeak
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) // wait for wifi to connect
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Get measurement from the senor(s)
  DataAcquistion();

  SendToThingSpeak(temp, hum, bar, audio);
  // For debugging 
  printValues();

  //delay(15000); // if not using powerSavingMode();
  powerSavingMode();
}

void loop()
{

}

void DataAcquistion()
{
  temp = bme.readTemperature();
  hum  = bme.readHumidity();
  bar  = bme.readPressure() / 100.0f;
  audio = analogRead(A0);
}

void SendToThingSpeak(float l_temp, float l_hum, float l_bar, int l_audio)
{
  // put values into each field to be sent to ThingSpeak as a message
  ThingSpeak.setField(1, l_temp);
  ThingSpeak.setField(2, l_hum);
  ThingSpeak.setField(3, l_bar);
  ThingSpeak.setField(4, l_audio);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) // success value from ThingSpeak
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(10000);
}

void printValues()
{
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bar);
  Serial.println(" hPa");
/* 
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
*/
  Serial.print("Humidity = ");
  Serial.print(hum);
  Serial.println(" %");

  Serial.print("Audio = ");
  Serial.println(audio);
  Serial.println();
}

void powerSavingMode()
{
  //Sleep for 30 seconds.
  ESP.deepSleep(30e6);

  //Sleep for ~3h.50m 
  //ESP.deepSleep(ESP.deepSleepMax())
  
}
