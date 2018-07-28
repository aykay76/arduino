void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("ESP8266 in normal mode");

  delay(3000);
  ESP.deepSleep(60 * 1000000);

  Serial.println("You'll never see this");
}

void loop() {
  // never gets called

}
