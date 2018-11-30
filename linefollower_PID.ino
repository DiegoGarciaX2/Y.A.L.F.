#include <PID_v1.h>
#include <Wire.h>
#define uchar unsigned char
uchar t;
int calib[8];
uchar data[16];
double Kp=0.25, Ki=10, Kd=5 setpoint, va, vb, pos;
PID motorA(&pos, &va, &setpoint, Kp, Ki, Kd, DIRECT);
PID motorB(&pos, &vb, &setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  calibrateSensors(calib);
  setpoint = 0;
  pos=0;
  motorA.SetMode(AUTOMATIC);
  motorB.SetMode(AUTOMATIC);
  t = 0;
}
void loop() {
  int sensor[8];
  getSensorValues(sensor);
  transformBinary(sensor, calib);
  pos = linePos(sensor);
  Serial.print("Position value: ");
  Serial.print(pos);
  Serial.println();
  motorA.Compute();
  Serial.print("Speed motor A:  ");
  Serial.print(va);
  Serial.println();
  motorB.Compute();
  Serial.print("Speed motor B:  ");
  Serial.print(vb);
  Serial.println();
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
  for (int a = 0; a < 8 ; a++) {
    sensorHigh[a] = 0;
    sensorLow[a] = 255;
  }
  int rep = 1;
  while (rep < 50) {
    delay(100);
    getSensorValues(sensor);
    for (int a = 0; a < 8; a++) {
      if (sensor[a] < sensorLow[a] && sensor[a] > 0) {
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
    calibrationValues[a] = (sensorHigh[a] - sensorLow[a])/2;
    Serial.print(calibrationValues[a]);
    Serial.print("  ");
  }
  Serial.println();
}

void transformBinary(int sensor[], int calib[]) {
  Serial.print("Sensor binary: ");
  for (int a = 0; a < 8; a++) {
    sensor[a]=(sensor[a] > calib[a]) ? 0 : 1;
    Serial.print(sensor[a]);
    Serial.print("  ");
  }
  Serial.println();
}

int linePos(int sensor[]) {
  int k;
  for (int a = 0; a < 8; a++) {
    if (sensor[a] == 0 && sensor[a + 1] == 0) {
      k = a - 3;
    }
  }
  return k;
}

