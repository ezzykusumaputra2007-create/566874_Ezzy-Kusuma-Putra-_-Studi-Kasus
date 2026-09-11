//Rizqi Prawira Wisesa (25/560504/PA/23621)
//Ezzy Kusuma Putra (25/566874/PA/23929)
#include <Arduino.h>

#define LED_BLINK 33

void setup(){
  Serial.begin(115200);

  pinMode(LED_BLINK, OUTPUT);
  Serial.println("1.Output Digital (LED Blink)");
}

void loop(){
  digitalWrite(LED_BLINK, HIGH);
  delay(3000);

  digitalWrite(LED_BLINK, LOW);
  delay(3000);
}