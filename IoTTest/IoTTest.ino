/*************************************************************************************************************************************************
 *  Sketch developed by User: CompEng0001
 *  Creation Date: 12/07/2019
 *  Verison: 1.4
 *  Hardware: Arduino MKR 1010 uC, Dust sensor, Gas (MQ5) sensor, Light sensors, BMP280, Audio
 *  Documentation: https://github.com/CompEng0001/IoT-and-Our-Schools-Enivronments-for-Education-
 *  Licence: Attribution-NonCommercial-ShareAlike 4.0 International [CC BY-NC-SA 4.0] (Where licences for hardware and software do not conflict)
**************************************************************************************************************************************************/

// Setup variables to be used

#include <WiFi.h>
#include <math.h>
#include <Adafruit_BMP280.h>
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
#define GAS_SENSOR A0
float gasValue = 0.0;

// BMP280 Sensor variables
float temp, alt, bar;

// BigSound Sensor variable
#define AUDIO_SENSOR A2
long audio = 0 ;

// Light Sensor variables
#define LIGHT_SENSOR A4              //Grove - Light Sensor is connected to A4
of Arduino
const int ledPin = 12;               //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue = 10;       //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
int lightLevel;

WiFiClient client;                    // create instance of WiFiClient
Adafruit_BMP280 bmp;                  // I2C

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT);                // Set Dust Sensor pin to INPUT

  connectivity();                     // for BMP280(I2C) WiFi and Thingspeak

  starttime = millis();               //get the current time;
}

// this runs forever, or until the microController is turned off.
void loop() 
{
  dataAcquisition();                   // get data from sensors 
  displayData();
 // sendToThingSpeak(temp, hum, bar, audio, concentration, lightLevel, gasValue );              // Send data from sensors to thingspeak
  delay(1500);                        // minimum delay(15000) to send next set of data
}

/**
 * dataAcquisition is acquires data for the 5 sensors
 * @sensor Dust Sensor (concentration)
 * @sensor MQ5 Gas Sensor
 * @sensor BMP280 (Barometer, Temperature and Humidity)
 * @sensor Light sensor (intensity of light)
 * @sensor Audio sensor (amplitude of sound)
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

    gasValue = analogRead(GAS_SENSOR);                      // Get the reading from the GAS_SENSOR 

    lightLevel = analogRead(LIGHT_SENSOR);                  // Get the reading from the LIGHT_SENSOR 

    temp = bmp.readTemperature();                           // Get the Temperature reading from the BMP280
    alt  = bmp.readAltitude(1013.25);                              // Get the Humidity reading from the BMP280
    bar  = bmp.readPressure() /100.0f;                     // Get the Pressure reading from the BMP280

    audio = 0;
    for(int i=0; i<32; i++)
    {
        audio += analogRead(AUDIO_SENSOR);
    }

    audio >>= 5;
   // audio = analogRead(AUDIO_SENSOR);                         // Get the Audio amplitude from the AUDIO_SENSOR

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
    Serial.println(" pcs/0.01cf");                            // Particles/per 0.1cubic feet
    Serial.println();                                     // New line escape character

    //Print gas value
    Serial.print("Gas ADC = ");
    Serial.println(gasValue);
    Serial.println();

    //Print light level
    Serial.print("the Light level ADC is ");
    Serial.println(lightLevel);
    Serial.println();

    //Print BMP280 data
    Serial.print("The pressure is: ");
    Serial.println(bar);
    Serial.print("The Temperature is: ");
    Serial.println(temp);
    Serial.print("The Altitude is: ");
    Serial.println(alt);
    Serial.println();

    //Print Audio
    Serial.print("The intensity of sound is: ");
    Serial.println(audio);
    Serial.println();
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
  Serial.begin(115200);                               // set the baud rate, the number of bits per second
  delay(10);                                          // give serial chance to settle
Serial.println("HERE");
                                      // Initialized false by default
  if(!bmp.begin())                                     // while not true attempt connection to BMP280
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    delay(500);
    Serial.print(".");
    while(1);
  }
  Serial.println("");
  Serial.println("BMP280 is connected");

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
                  
/*
  Serial.println("Connecting to ");                   // debugging purposes
  Serial.println(ssid);
  ThingSpeak.begin(client);                           // Initialize ThingSpeak
  WiFi.begin(ssid, pass);                             // Initialize Wifi module with the connection credentials

  while (WiFi.status() != WL_CONNECTED)               // do this until connected to Wifi
  {
    delay(500);
    Serial.print(".");
  }s
  Serial.println("");
  Serial.println("WiFi connected");
  */
}
