/************************************************************************************************
 *  Sketch developed by User: CompEng0001
 *  Creation Date: 12/07/2019
 *  Verison: 1.4
 *  Documentation: 
**************************************************************************************************/

// Setup variables to be used
// Simple code upload the tempeature and humidity data using thingspeak.com
// Hardware: Arduino MKR 1010 uC, Dust sensor, Gas (MQ5) sensor, Light sensors, BMP280 ....
#include <WiFi.h>
#include <math.h>
#include <Adafruit_BME280.h>
#include <ThingSpeak.h> // downlaod the library - ThingSpeak

//WiFi and Thingspeak variables
char ssid[] =   "SSID";     // replace with your wifi ssid and wpa2 key
char pass[] =  "PASSWORD";
unsigned long myChannelNumber = 123456; // ThingSpeak Channel number
const char * myWriteAPIKey = "YOURAPI"; //  Enter your Write API key from ThingSpeak`

// Dust Sensor variables 
int pin = 0;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

// MQ5 Sensor variables
#define GAS_SENSOR = A0;
float gasValue = 0.0;

// BME280 Sensor variables
float temp, hum, bar;

// BigSound Sensor variable
#define AUDIO_SENSOR = A2;
int audio;


// Light Sensor variables
#define LIGHT_SENSOR A1              //Grove - Light Sensor is connected to A0 of Arduino
const int ledPin = 12;               //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue = 10;       //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
int lightLevel;

WiFiClient client;                    // create instance of WiFiClient
Adafruit_BME280 bme;                  // I2C

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT);                // Set Dust Sensor pin to INPUT

  connectivity();                     // for BME280(I2C) WiFi and Thingspeak

  starttime = millis();               //get the current time;
}

// this runs forever, or until the microController is turned off.
void loop() 
{
  dataAcquisition();                   // get data from sensors 

  sendToThingSpeak(temp, hum, bar, audio, concentration, lightLevel, gasValue );              // Send data from sensors to thingspeak
  delay(15000);                        // minimum delay(15000) to send next set of data
}

/**
 * dataAcquisition is acquires data for the 5 sensors
 * @sensor Dust Sensor
 * @sensor MQ5 Gas Sensor
 * @sensor BMP280 (Barometer, Temperature and Humidity)
 * @sensor Light sensor (intensity of light)
 * @sensor Audio sensor (amplitude of noise)
 */
void dataAcquisition()
{
  duration = pulseIn(pin, LOW);                             // Set dust pin to low then check if another time has passed
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - starttime) >= sampletime_ms)              //if the sample time >= 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0);     // Integer percentage 0=&gt;100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;   // using spec sheet curve
    lowpulseoccupancy = 0;

    gasValue = analogRead(GAS_SENSOR);

    lightLevel = analogRead(LIGHT_SENSOR);

    temp = bme.readTemperature();
    hum  = bme.readHumidity();
    bar  = bme.readPressure() / 100.0f;
    audio = analogRead(AUDIO_SENSOR);

    displayData();                                          // Comment to disable debugging to serial monitor
    starttime = millis();                                   // starttime should now be current time 
  }
}

/**
 * Display all sensor data on to serial monitor for debugging purposes
 */
void displayData()
{
  // Print dust concentration
    Serial.print("concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n"); // New line escape character

    //Print gas value
    Serial.print("Gas ADC = ");
    Serial.println(gasValue);
    Serial.println("\n");

    //Print light level
    Serial.print("the Light level ADC is ");
    Serial.println(lightLevel);
    Serial.println("\n");

    //Print BMP280 data
    Serial.print("The pressure is: ");
    Serial.println(bar);
    Serial.print("The Temperature is: ");
    Serial.println(temp);
    Serial.print("The Humidity is: ");
    Serial.println(hum);
    Serial.println("/n");
}

/**
 * sendToThingSpeak function takes 7 variables and sends them to thingspeak this matches number of fields in the thingspeak channel you created.
 * @param Temperature
 * @param Humidity
 * @param Pressure
 * @param Audio
 * @param Dust
 * @param Light
 * @Param Gas
 */
void sendToThingSpeak(float l_temp, float l_hum, float l_bar, int l_audio, float l_dust, int l_light, float l_gas)
{
  // this formats the data into the accepted format for thingspeak to process 
  ThingSpeak.setField(1, l_temp);
  ThingSpeak.setField(2, l_hum);
  ThingSpeak.setField(3, l_bar);
  ThingSpeak.setField(4, l_audio);
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
}

/**
 * connectivity function sets up the connection to BMP280 and WiFi
 */
void connectivity()
{
  Serial.begin(115200);                               //set the baud rate, the number of bits per second
  delay(10);                                          // give serial chance to settle

  bool status;                                        // Initialized false by default
  while (!status)                                     // while not true
  {
    status = bme.begin(0x76);                         // is this address true?
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("BMP280 is connected");


  Serial.println("Connecting to ");                   // debugging purposes
  Serial.println(ssid);
  ThingSpeak.begin(client);                           // Initialize ThingSpeak
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)               // wait for wifi to connect
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}