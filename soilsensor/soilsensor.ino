#include <time.h>
#include <ESP8266WiFi.h>

String ssid                         = "BTHub5-MH2Q";        // your network SSID (name)
String pass                         = "52c2ea838d";  // your network password (use for WPA, or use as key for WEP)
unsigned long ms = 0;
WiFiClient wifiClient;

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A0));

  if (!wifiClient.connect("192.168.1.144", 9200)) {
    Serial.println("Connection failed!");
  }
  //wifiClient.print(String("POST /office/reading HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: ") + data.length() + "\r\n\r\n" + data);
  
  delay(1000);
}
