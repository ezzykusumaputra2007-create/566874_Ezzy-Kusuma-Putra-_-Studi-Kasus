
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_INA219 ina219;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "L";
const char* password = "28042007";

String apiKey = "kelompokk1";

const int pinPot = 34;

const int pinPWMA = 25;
const int pinAIN1 = 26;
const int pinAIN2 = 27;
const int pinSTBY = 14;

const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;

const unsigned long intervalThingspeak = 20000;
unsigned long lastSend = 0;

int nilaiPot = 0;
int pwmPercent = 0;
int nilaiPWM = 0;

float arus_mA = 0;
float tegangan_V = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin();

  pinMode(pinPot, INPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinSTBY, OUTPUT);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(pinPWMA, pwmChannel);

  digitalWrite(pinSTBY, HIGH);

  ina219.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Koneksi WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Terhubung!");
}

void loop() {
  nilaiPot = analogRead(pinPot);

  pwmPercent = map(nilaiPot, 0, 4095, 0, 100);
  nilaiPWM = map(nilaiPot, 0, 4095, 0, 255);

  digitalWrite(pinAIN1, HIGH);
  digitalWrite(pinAIN2, LOW);

  ledcWrite(pwmChannel, nilaiPWM);

  arus_mA = ina219.getCurrent_mA();
  tegangan_V = ina219.getBusVoltage_V();

  Serial.print("PWM: ");
  Serial.print(pwmPercent);
  Serial.print("% | Arus: ");
  Serial.print(arus_mA);
  Serial.print(" mA | Tegangan: ");
  Serial.print(tegangan_V);
  Serial.println(" V");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("MONITORING MOTOR DC");

  display.setCursor(0, 16);
  display.print("PWM: ");
  display.print(pwmPercent);
  display.println(" %");

  display.setCursor(0, 32);
  display.print("Arus: ");
  display.print(arus_mA);
  display.println(" mA");

  display.setCursor(0, 48);
  display.print("Tegangan: ");
  display.print(tegangan_V);
  display.println(" V");

  display.display();

  if (millis() - lastSend >= intervalThingspeak) {
    lastSend = millis();

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String url = "http://api.thingspeak.com/update?api_key="
                   + apiKey
                   + "&field1=" + String(pwmPercent)
                   + "&field2=" + String(arus_mA)
                   + "&field3=" + String(tegangan_V);

      http.begin(url);

      int httpCode = http.GET();
      String respon = http.getString();

      http.end();

      if (httpCode == 200 && respon != "0") {
        Serial.println("Data berhasil dikirim!");
        Serial.println("Entry ke-" + respon);
      } else {
        Serial.print("Gagal mengirim data: ");
        Serial.println(httpCode);
      }
    } else {
      Serial.println("WiFi terputus!");
    }
  }

  delay(200);
}
