/*
   Sketch developed by User: CompEng0001
   Creation Date: 21/05/2019
   Verison: 1.0.8
*/

// Setup variables to be used
// Simple code upload the tempeature and humidity data using thingspeak.com
// Hardware: NodeMCU uC, DHT22 sensor, moisture sensor, ....

#include <ESP8266WiFi.h> // needed for the NodeMcu
#include <DHTesp.h> // download the library - DHT sensor library for ESPx
#include "ThingSpeak.h" // downlaod the library - ThingSpeak

char ssid[] =  "Your SSID";     // replace with your wifi ssid and wpa2 key
char pass[] =  "Your SSID Password";
unsigned long myChannelNumber = 1234567; // ThingSpeak Channel number
const char * myWriteAPIKey = "YourAPIKey"; //  Enter your Write API key from ThingSpeak`

float temp, hum; // variables for DHT22

int moisture =0, audio=0; // soil Sensor values

#define DHTPIN 0        //pin where the dht11 is connected D3
#define audioPIN 14 // D5

DHTesp dht; // create instance of DHTesp
WiFiClient client; // create instance of WiFiClient

void setup()
{
  Serial.begin(115200); //set the baud rate, the number of bits per second
  delay(10); // give serial chance to settle
  dht.setup(DHTPIN, DHTesp::DHT11); // begin the dht service and get DHT11 
  pinMode(audioPIN,INPUT);

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

void loop()
{
  if(!Serial.available())
  {
    Serial.begin(115200); //set the baud rate, the number of bits per second
    delay(10); // give serial chance to settle

    dht.setup(DHTPIN, DHTesp::DHT11); // begin the dht service and get DHT11
  }
  
   // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  
  DataAcquistion();
  SendToThingSpeak(temp, hum, audio);
  delay(15000);
  //Sleep();
}

/*
 * Put all DAQ here to keep code clean
 */
void DataAcquistion()
{
  //DHT22 data
  delay(dht.getMinimumSamplingPeriod());
  hum  = dht.getHumidity();
  temp = dht.getTemperature(); 

  //audio Data
  audio = analogRead(A0);

/*
  Serial.print("Temperature is = ");
  Serial.println(temp);
  Serial.print("humidity is = ");
  Serial.println(hum);
  Serial.print("Audio is = ");
  Serial.println(audio);
  */
}

/*
 * This function sends data to thingspeak, each input parameter matches the vars in DataAcquistion()
 * Rememeber you can only send at most 8 fields at once to one chanel.
 */
void SendToThingSpeak(float l_temp, float l_hum, int l_audio)
{
  // put values into each field to be sent to ThingSpeak as a message
  ThingSpeak.setField(1, l_temp);
  ThingSpeak.setField(2, l_hum);
  ThingSpeak.setField(3, l_audio);

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
void powerSavingMode()
{
  //Sleep for 30 seconds.
  ESP.deepSleep(30e6);

  //Sleep for ~3h.50m 
  //ESP.deepSleep(ESP.deepSleepMax())
}
