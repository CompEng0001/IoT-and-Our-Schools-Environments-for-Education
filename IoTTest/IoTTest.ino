/*
   Sketch developed by User: CompEng0001
   Creation Date: 21/05/2019
   Verison: 1.0.9
*/

// Setup variables to be used
// Simple code upload the tempeature and humidity data using thingspeak.com
// Hardware: Arduino MKR 1010 uC, Dust sensor, Gas (MQ5) sensor, Light sensors, BME280 ....
#include <WiFi.h>
#include <math.h>
#include <Adafruit_BME280.h>
#include <ThingSpeak.h> // downlaod the library - ThingSpeak

char ssid[] =   "SSID";     // replace with your wifi ssid and wpa2 key
char pass[] =  "PASSWORD";
unsigned long myChannelNumber = 123456; // ThingSpeak Channel number
const char * myWriteAPIKey = "YOURAPI"; //  Enter your Write API key from ThingSpeak`


// Dust Sensor variables 
int pin = 0;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000;//sampe 30s&nbsp;;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;


// MQ5 Sensor variables
float gasValue = 0.0;
float gasSensor = A0;

// BME280 Sensor variables
float temp, hum, bar;

// BigSound Sensor variable
int audio;

// Light Sensor variables
#define LIGHT_SENSOR A0//Grove - Light Sensor is connected to A0 of Arduino
const int ledPin = 12;               //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue = 10;       //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
int lightLevel;

WiFiClient client; // create instance of WiFiClient
Adafruit_BME280 bme; // I2C

void setup()
{

  pinMode(pin, INPUT); // Set Dust Sensor pin to INPUT

  connectivity(); // for BME280(I2C) WiFi and Thingspeak

  starttime = millis();//get the current time;
}

void loop() {

  dataAcquisition(); // get data from sensors 

  sendToThingSpeak(temp, hum, bar, audio, concentration, lightLevel, gasValue ); // Send data from sensors to thingspeak
  delay(15000); // minimum delay(15000) to send next set of data

}


void dataAcquisition()
{

  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0); // Integer percentage 0=&gt;100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
    Serial.print("concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;

    gasValue = analogRead(gasSensor);
    Serial.print("Gas ADC = ");
    Serial.println(gasValue);

    lightLevel = analogRead(LIGHT_SENSOR);
    Serial.print("the Light level Analogue is ");
    Serial.println(lightLevel);

    starttime = millis();
  }

  temp = bme.readTemperature();
  hum  = bme.readHumidity();
  bar  = bme.readPressure() / 100.0f;
  audio = analogRead(A0);


}

void sendToThingSpeak(float l_temp, float l_hum, float l_bar, int l_audio, float l_dust, int l_light, float l_gas)
{
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
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(10000);
}

void connectivity()
{
  Serial.begin(115200); //set the baud rate, the number of bits per second
  delay(10); // give serial chance to settle

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


  Serial.println("Connecting to "); // debugging purposes
  Serial.println(ssid);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) // wait for wifi to connect
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}
