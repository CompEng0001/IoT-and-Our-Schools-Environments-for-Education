/*************************************************************************************************************************************************
    Sketch developed by User: CompEng0001
    Creation Date: 12/07/2019
    Verison: 1.4
    Hardware: Arduino MKR 1010 uC, Dust sensor, Gas (MQ5) sensor, Light sensors, BMP280, Audio
    Documentation: https://github.com/CompEng0001/IoT-and-Our-Schools-Enivronments-for-Education-
    Licence: Attribution-NonCommercial-ShareAlike 4.0 International [CC BY-NC-SA 4.0] (Where licences for hardware and software do not conflict)
**************************************************************************************************************************************************/

// Setup variables to be used
#include <WiFiNINA.h>        // enable WiFi for MKR1010
#include <math.h>            // for some math functions
#include <seeed_bme680.h>    // for BME680 sensor
#include <ThingSpeak.h>      // downlaod the library - ThingSpeak
#include <ArduinoLowPower.h>   // enable powersaving 

//WiFi and Thingspeak variables
char ssid[] = "BT-QXA277"; // replace with your wifi ssid and wpa2 key
char pass[] = "G67pDKimraqUpQ";
unsigned long myChannelNumber = 792104;         // ThingSpeak Channel number
const char *myWriteAPIKey = "B1NL1Z3AL4Q8ALD5"; //  Enter your Write API key from ThingSpeak`

// Dust Sensor variables
int pin = 0;
unsigned long duration;
unsigned long starttime;
float concentration = 0;

// MQ5 Sensor variables
#define GAS_SENSOR A0
float gasValue = 0.0;

// BME680 Sensor variables
float temp, hum, bar, voc, IAQ_Score;
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define IIC_ADDR  uint8_t(0x76)

// Light Sensor variables
#define LIGHT_SENSOR A4        //Grove - Light Sensor is connected to A4of Arduino
const int ledPin = 12;         //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue = 10; //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
int lightLevel, counter = 0;

WiFiClient client;   // create instance of WiFiClient
Seeed_BME680 bme680(IIC_ADDR); //IIC

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT); // Set Dust Sensor pin to INPUT
  setupConnectivity();
}

// this runs forever, or until the microController is turned off.
void loop()
{
  dataAcquisition(); // get data from sensors
  displayData();
  Serial.print("Total Seconds ===> ");
  Serial.println((counter+= 30));
  wiFiController();
  sendToThingSpeak(temp, bar, hum, IAQ_Score, concentration, lightLevel, gasValue);
}

/**
  dataAcquisition is acquires data for the 5 sensors
  @sensor Dust Sensor (concentration)
  @sensor MQ5 Gas Sensor
  @sensor BMP680 (Barometer, Temperature,Humidity and VOC)
  @sensor Light sensor (intensity of light)
*/
void dataAcquisition()
{
  getDustConcentration(); // this takes 30 seconds
  getGasData();           // Get the reading from the GAS_SENSOR
  getLightLevels();
  getBMEValues();
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
  sensor_volt = (float)sensorValue / 1024 * 5.0;
  RS_gas = (5.0 - sensor_volt) / sensor_volt; // omit *RL
  float RO = RS_gas / 6.5;
  /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
  gasValue = RS_gas / RO; // ratio = RS/R0
}

/**
  getLightLevels gets the light Intensity
  0 dark 10 blinding
  note sensor operates in 540nm range so light is composed of green frequency
*/
void getLightLevels()
{
  lightLevel = analogRead(LIGHT_SENSOR);
  lightLevel = map(lightLevel, 0, 900, 0, 10); // Get the reading from the LIGHT_SENSOR and turn map from high to low
}

/**

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
    for(int i=0; i<100;i++){
  voc += bme680.sensor_result_value.gas;
  
  }
  voc /= 100;
  int hum_ref = 40, gas_ref = 0, gasLL = 5000, gasUL = 50000;
  float humscore=0.00, gasscore=0.00;

  // Get humidity Ref

  if(hum >= 38 && hum <=42) { humscore = 0.25 *100; }
  else if (hum < 38) { humscore = ((0.25/hum_ref)*hum)*100; }
  else { humscore = (((-0.25/(100-hum_ref))*hum)+0.416666)*100; }
    
  // Get VOC ref
  if(voc >= gasUL) { gas_ref = gasUL; } 
  else if (voc <= gasLL) { gas_ref = gasLL; }
  else { gas_ref = (int)voc;}
  gasscore = (0.75/(gasUL-gasLL)*gas_ref - (gasLL*(0.75/(gasUL-gasLL))))*100;
  
  // Get IAQ score
  IAQ_Score = (100-(humscore+gasscore))*5;
}

/**

*/
void getDustConcentration()
{
  bool data = false;
  unsigned long lowpulseoccupancy = 0;
  unsigned long sampletime_ms = 30000; //sampe 30s
  
  while (!data)
  {
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
    if ((millis() - starttime) > sampletime_ms) //if the sampel time == 30s
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

  //Print gas value
  Serial.print("Air Quality = ");
  Serial.println(gasValue);

  //Print light level
  Serial.print("the Light level ADC is ");
  Serial.println(lightLevel);

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
  Serial.println(IAQ_Score);
  Serial.println();
}

/**
  sendToThingSpeak function takes 7 variables and sends them to thingspeak this matches number of fields in the thingspeak channel you created.
  @param Temperature
  @param Pressure
  @param humidity
  @param VOC
  @param Dust
  @param Light
  @Param Gas
*/
void sendToThingSpeak(float l_temp, float l_bar, float l_hum, float l_IAQ, float l_dust, int l_light, float l_gas)
{
  // this formats the data into the accepted format for thingspeak to process
  ThingSpeak.setField(1, l_temp);
  ThingSpeak.setField(2, l_bar);
  ThingSpeak.setField(3, l_hum);
  ThingSpeak.setField(4, l_IAQ);
  ThingSpeak.setField(5, l_dust);
  ThingSpeak.setField(6, l_light);
  ThingSpeak.setField(7, l_gas);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x)); // will return a number for referencing the problem
  }

  WiFi.disconnect();
  WiFi.lowPowerMode();
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

  Serial.println("");
  Serial.println("BMP280 is connected");

  ThingSpeak.begin(client); // Initialize ThingSpeak

}

void wiFiController()
{
  WiFi.noLowPowerMode();

  while (WiFi.status() != WL_CONNECTED) // do this until connected to Wifi
  {
    WiFi.begin(ssid, pass);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
