void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(16, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(16));
  delay(100);
}
