#include <time.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

String ssid                         = "BTHub5-MH2Q";        // your network SSID (name)
String pass                         = "52c2ea838d";  // your network password (use for WPA, or use as key for WEP)
unsigned long ms = 0;
WiFiClient wifiClient;
WiFiUDP udp;
IPAddress IP_Broadcast(192, 168, 1, 144);

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
  //      ms = millis();
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWifi();
  //pinMode(LED_BUILTIN, OUTPUT);
  udp.begin(1421);
}

void loop() {
  // put your main code here, to run repeatedly:
  char buffer[80];
  int value = analogRead(A0);
  uint8_t mapped = 100 - map(value, 0, 1024, 0, 100);
  sprintf(buffer, "%d => %d", value, mapped);
  Serial.println(buffer);

  if (udp.beginPacket(IP_Broadcast, 1421))
  {
    if (udp.write(mapped) == 0)
    {
      Serial.println("Error writing to UDP packet");    
    }
    if (udp.endPacket() == 0)
    {
      Serial.println("Error sending broadcast");
    }
  }
  else
  {
    Serial.println("Can't begin UDP broadcast");
  }

  udp.beginPacket(IP_Broadcast, 1421);
  if (udp.write((uint8_t *)"Hello", 5) == 0)
  {
    Serial.println("Error writing to UDP packet...");    
  }
  udp.endPacket();

  String data = "{\"soilmoist\": " + String(mapped) + "}";
  
  if (wifiClient.connect("cubi.home", 9200)) {
    wifiClient.print(String("POST /plant/reading HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: ") + data.length() + "\r\n\r\n" + data);
  } else {
    Serial.println("Connection failed!");
  }
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
  delay(1000);
}
