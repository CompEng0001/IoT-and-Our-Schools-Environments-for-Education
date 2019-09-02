/*************************************************************************************************************************************************
    Sketch developed by User: CompEng0001
    Creation Date: 12/07/2019
    Verison: 1.4
    Hardware: Arduino MKR 1010 uC, Dust sensor, Gas (MQ5) sensor, Light sensors, BMP280, Audio
    Documentation: https://github.com/CompEng0001/IoT-and-Our-Schools-Enivronments-for-Education-
    Licence: Attribution-NonCommercial-ShareAlike 4.0 International [CC BY-NC-SA 4.0] (Where licences for hardware and software do not conflict)
**************************************************************************************************************************************************/

// Setup variables to be used
#include <WiFiNINA.h> // enable WiFi for MKR1010
#include <math.h> // for some math functions
#include <Adafruit_BMP280.h> // for BMP280 sensor
#include <ThingSpeak.h> // downlaod the library - ThingSpeak

//WiFi and Thingspeak variables
char ssid[] =   "CompEng0001";     // replace with your wifi ssid and wpa2 key
char pass[] =  "12345678!";
unsigned long myChannelNumber = 792104; // ThingSpeak Channel number
const char * myWriteAPIKey = "B1NL1Z3AL4Q8ALD5"; //  Enter your Write API key from ThingSpeak`

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

// Light Sensor variables
#define LIGHT_SENSOR A4              //Grove - Light Sensor is connected to A4of Arduino
const int ledPin = 12;               //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue = 10;       //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
int lightLevel;

WiFiClient client;                    // create instance of WiFiClient
Adafruit_BMP280 bmp;                  // I2C

// this function runs once everytime the microController is turned on.
void setup()
{
  pinMode(pin, INPUT);                // Set Dust Sensor pin to INPUT
  setupConnectivity();
}

// this runs forever, or until the microController is turned off.
void loop()
{
  dataAcquisition();                   // get data from sensors
  displayData();
  wiFiController();
  sendToThingSpeak(temp, bar, alt, audio, concentration, lightLevel, gasValue);
  delay(30000);        // for BMP280(I2C) WiFi and Thingspeak
}

/**
  dataAcquisition is acquires data for the 5 sensors
  @sensor Dust Sensor (concentration)
  @sensor MQ5 Gas Sensor
  @sensor BMP280 (Barometer, Temperature and Humidity)
  @sensor Light sensor (intensity of light)
  @sensor Audio sensor (amplitude of sound)
*/
void dataAcquisition()
{
  getDustConcentration();
  getGasData();                                           // Get the reading from the GAS_SENSOR
  getLightLevels();
  getBMPValues();
  getAudioIntensity();
}

/**
  getGasData acquires the gas ratio which indicates what gas is being detected
  TODO - find the relationship between RS/RO for each gas in PPM?
*/
void getGasData()
{
  float sensor_volt;
  float RS_gas; // Get value of RS in a GAS
  float ratio; // Get ratio RS_GAS/RS_air
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

  lightLevel = map(lightLevel, 0, 900, 0, 10);// Get the reading from the LIGHT_SENSOR and turn map from high to low
}

/**

*/
void getBMPValues()
{
  temp = bmp.readTemperature();                           // Get the Temperature reading from the BMP280
  alt  = abs(bmp.readAltitude(1013.25));                              // Get the Humidity reading from the BMP280
  bar  = bmp.readPressure() / 100.0f;                    // Get the Pressure reading from the BMP280
}

void getDustConcentration()
{
  duration = pulseIn(pin, LOW);                             // Set dust pin to low then check if another time has passed
  lowpulseoccupancy = lowpulseoccupancy + duration;
  ratio = lowpulseoccupancy / (sampletime_ms * 10.0);     // Integer percentage 0=&gt;100
  concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;   // using spec sheet curve
  lowpulseoccupancy = 0;
}

/**
  Display all sensor data on to serial monitor for debugging purposes
*/
void displayData()
{
  // Print dust concentration
  Serial.print("concentration = ");
  Serial.print(concentration);
  Serial.println(" pcs/0.01cf");                            // Particles/per 0.1cubic feet
  Serial.println();                                     // New line escape character

  //Print gas value
  Serial.print("Air Quality = ");
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

}

/**
   sendToThingSpeak function takes 7 variables and sends them to thingspeak this matches number of fields in the thingspeak channel you created.
   @param Temperature
   @param Pressure
   @param Altitude
   @param Dust
   @param Light
   @Param Gas
*/
void sendToThingSpeak(float l_temp, float l_bar, float l_alt, float l_dust, int l_light, float l_gas)
{
  // this formats the data into the accepted format for thingspeak to process
  ThingSpeak.setField(1, l_temp);
  ThingSpeak.setField(2, l_bar);
  ThingSpeak.setField(3, l_alt);
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
  Serial.begin(115200);                               // set the baud rate, the number of bits per second
  delay(1000);                                          // give serial chance to settle
  Serial.println("HERE");
  // Initialized false by default
  if (!bmp.begin())                                    // while not true attempt connection to BMP280
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    delay(500);
    Serial.print(".");
    while (1);
  }
  Serial.println("");
  Serial.println("BMP280 is connected");

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */


  Serial.println("Connecting to ");                    // debugging purposes

  ThingSpeak.begin(client);                            // Initialize ThingSpeak
  Serial.println(ssid);                                // Initialize Wifi module with the connection credentials
}

void wiFiController()
{
  WiFi.noLowPowerMode();

  while (WiFi.status() != WL_CONNECTED)                // do this until connected to Wifi
  {
    WiFi.begin(ssid, pass);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}
