// Incorporates: Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>

#include "DHT.h"

String ssid                         = "BTHub5-MH2Q";        // your network SSID (name)
String pass                         = "52c2ea838d";  // your network password (use for WPA, or use as key for WEP)

#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
unsigned long ms = 0;

void initWifi() {
    if (WiFi.status() != WL_CONNECTED) 
    {
        WiFi.stopSmartConfig();
        WiFi.enableAP(false);

        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        WiFi.begin(ssid.c_str(), pass.c_str());
    
        Serial.println("Waiting for Wifi connection.");
        while (WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
            delay(500);
        }
    
        Serial.println("Connected to wifi");

        initTime();
        ms = millis();
    }
}

void initTime() {
    time_t epochTime;

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true) {
        epochTime = time(NULL);

        if (epochTime == 0) {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        } else {
            Serial.print("Fetched NTP epoch time is: ");
            Serial.println(epochTime);
            break;
        }
    }
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  initWifi();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  time_t timestamp = time(NULL);
  String data = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + ", \"timestamp\": " + String(timestamp) + "000}";
  Serial.println(data);

  if (!wifiClient.connect("192.168.1.144", 9200)) {
    Serial.println("Connection failed!");
  }
  wifiClient.print(String("POST /office/reading HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: ") + data.length() + "\r\n\r\n" + data);
  delay(50);
  bool first = true;
  while (wifiClient.available())
  {
      String line = wifiClient.readStringUntil('\r');
      if (first)
      {
        Serial.println(line);
        first = false;
      }
  }

  ESP.deepSleep(60 * 1000000);
}

void loop() 
{
}
