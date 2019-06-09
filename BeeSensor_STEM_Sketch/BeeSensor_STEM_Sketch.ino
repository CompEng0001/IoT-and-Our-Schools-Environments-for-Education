/*
   Sketch developed by User: CompEng0001
   Creation Date: 21/05/2019
   Modification Date:
   Verison: 1.0
*/

// Setup variables to be used
// Simple code upload the tempeature and humidity data using thingspeak.com
// Hardware: NodeMCU uC, DHT22 sensor, moisture sensor, ....

#include <ESP8266WiFi.h> // needed for the NodeMcu
#include <DHTesp.h> // download the library - DHT sensor library for ESPx
#include "ThingSpeak.h" // downlaod the library - ThingSpeak

/*
 * 
 */
char ssid[] =   "YOUR SSID";     // replace with your wifi ssid and wpa2 key
char pass[] =  "YOUR SSID PASSWORD";
unsigned long myChannelNumber = Your Channel; // ThingSpeak Channel number
const char * myWriteAPIKey = "Your channel Api writekey"; //  Enter your Write API key from ThingSpeak`

float temp, hum; // variables for DHT22

int moisture =0; // soil Sensor values

#define DHTPIN 0        //pin where the dht11 is connected A0

DHTesp dht; // create instance of DHTesp
WiFiClient client; // create instance of WiFiClient

void setup()
{
  Serial.begin(115200); //set the baud rate, the number of bits per second
  delay(10); // give serial chance to settle
  dht.setup(DHTPIN, DHTesp::DHT11); // begin the dht service and get DHT11 

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
   // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
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
  SendToThingSpeak(temp, hum, moisture);
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

  //Moisture Data
  moisture = analogRead(A0);
  // We could do some range sorting and assign new values to indicated the level of moisture like 0-100%
}

/*
 * This function sends data to thingspeak, each input parameter matches the vars in DataAcquistion()
 * Rememeber you can only send at most 8 fields at once to one chanel.
 */
void SendToThingSpeak(float l_temp, float l_hum, int l_moisture)
{
  // put values into each field to be sent to ThingSpeak as a message
  ThingSpeak.setField(1, l_temp);
  ThingSpeak.setField(2, l_hum);
  ThingSpeak.setField(3, l_moisture);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
Serial.println("I AM HERE");
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(10000);
}
