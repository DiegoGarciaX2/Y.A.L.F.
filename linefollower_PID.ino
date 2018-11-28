#include <PID_v1.h>
#include <Wire.h>
#define uchar unsigned char
uchar t;
int calib[8];
uchar data[16];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  calibrateSensors(calib);
  t = 0;
}
void loop() {
  int sensor[8];
  getSensorValues(sensor);
  transformBinary(sensor, calib);
}





void getSensorValues(int sensor[]) {
  int x = 0;
  Wire.requestFrom(9, 16);
  while (Wire.available()) {
    data[t] = Wire.read();
    if (t % 2 == 0) {
      sensor[x] = data[t];
      Serial.print(sensor[x]);
      Serial.print("  ");
      x++;
    }
    if (t < 15) {
      t++;
    }
    else {
      t = 0;
      x = 0;
      Serial.println();
    }
  }
}


void calibrateSensors(int calibrationValues[]) {
  int sensor[8];
  int sensorLow[8];
  int sensorHigh[8];
  getSensorValues(sensor);
  for (int a = 0; a < 8; a++) {
    sensorHigh[a] = sensor[a];
    sensorLow[a] = sensor[a];
  }
  int rep = 1;
  while (rep < 50) {
    delay(100);
    getSensorValues(sensor);
    for (int a = 0; a < 8; a++) {
      if (sensor[a] < sensorLow[a] && sensorLow[a] > 0) {
        sensorLow[a] = sensor[a];
      }
      if (sensor[a] > sensorHigh[a] && sensor[a] < 255) {
        sensorHigh[a] = sensor[a];
      }
    }
    rep++;
  }
  Serial.print("Sensor high: ");
  for (int a = 0; a < 8; a++) {
    Serial.print(sensorHigh[a]);
    Serial.print("  ");
  }
  Serial.println();
  Serial.print("Sensor low: ");
  for (int a = 0; a < 8; a++) {
    Serial.print(sensorLow[a]);
    Serial.print("  ");
  }
  Serial.println();
  Serial.print("Calibration values:  ");
  for (int a = 0; a < 8; a++) {
    calibrationValues[a] = sensorHigh[a] - sensorLow[a];
    Serial.print(calibrationValues[a]);
    Serial.print("  ");
  }
  Serial.println();
}

void transformBinary(int sensor[], int calib[]) {
  for (int a = 0; a < 8; a++) {
    (sensor[a] > calib[a]) ? 1 : 0;
  }
}

