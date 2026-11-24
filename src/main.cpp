
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

Adafruit_MPU6050 mpu;

const int pinSDA = 21;
const int pinSCL = 22;
const int pwmPin = 18;

const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8;

const float batasSudut = 30.0;

float accYTerakhir = 0.0;
float sudutTerakhir = 0.0;
String statusBaru = "";

float bacaKemiringan(float &nilaiAccelY) {
    sensors_event_t a, g, temp;

    mpu.getEvent(&a, &g, &temp);

    nilaiAccelY = a.acceleration.y;

    float roll = atan2(a.acceleration.y, a.acceleration.z)
                 * 180.0 / PI;

    return abs(roll);
}

void cekKondisiMPU() {
    sudutTerakhir = bacaKemiringan(accYTerakhir);

    if (sudutTerakhir > batasSudut) {
        statusBaru = "OVERLOAD";
        ledcWrite(pwmChannel, 255);
    } else {
        statusBaru = "NORMAL";
        ledcWrite(pwmChannel, 0);
    }
}

void setup() {
    Serial.begin(115200);

    Wire.begin(pinSDA, pinSCL);

    if (!mpu.begin(0x68)) {
        Serial.println("Gagal menemukan sensor MPU6050!");
    }

    Serial.println("MPU6050 Siap!");

    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(pwmPin, pwmChannel);
}

void loop() {
    unsigned long waktuSekarang = millis();
    unsigned long waktuSebelumnya = 0;
    const unsigned long interval = 200;

    if (waktuSekarang - waktuSebelumnya >= interval) {
        waktuSebelumnya = waktuSekarang;

        cekKondisiMPU();

        Serial.print("Accel Y: ");
        Serial.print(accYTerakhir, 2);

        Serial.print(" m/s2 | Sudut Miring: ");
        Serial.print(sudutTerakhir, 1);

        Serial.print(" derajat | Status: ");
        Serial.println(statusBaru);
    }
}
