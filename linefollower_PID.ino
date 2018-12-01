#include <Statistic.h>
#include <PID_v1.h>
#include <Wire.h>
#define uchar unsigned char
uchar t;
int calib[8];
uchar data[16];
double setpoint, va, vb, pos;
double Kpa = 0.25, Kia = 10, Kda = 5, Kpb = 0.25, Kib = 10, Kdb = 5;
PID motorA(&pos, &va, &setpoint, Kpa, Kia, Kda, DIRECT);
PID motorB(&pos, &vb, &setpoint, Kpb, Kib, Kdb, DIRECT);
Statistic highSens[8];
Statistic lowSens[8];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  calibrateSensors(calib);
  setpoint = 0;
  pos = 0;
  motorA.SetMode(AUTOMATIC);
  motorB.SetMode(AUTOMATIC);
  t = 0;
  for (int a = 0; a < 8; a++) {
    highSens[a].clear();
    lowSens[a].clear();
  }
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
  Serial.println();
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
  delay(1000);
  getSensorValues(sensor);
  for (int a = 0; a < 8; a++) {
    lowSens[a].add(sensor[a]);
    highSens[a].add(sensor[a]);
  }
  int rep = 1;
  do {
    delay(100);
    getSensorValues(sensor);
    for (int a = 0; a < 8; a++) {
      if (sensor[a] < lowSens[a].average() && sensor[a] > 0) {
        lowSens[a].add(sensor[a]);
      }
      if (sensor[a] > highSens[a].average() && sensor[a] < 255) {
        highSens[a].add(sensor[a]);
      }
    }
    rep++;
  } while (rep < 50);

  Serial.print("Sensor high: ");
  for (int a = 0; a < 8; a++) {
    sensorHigh[a] = highSens[a].average();
    Serial.print(sensorHigh[a]);
    Serial.print("  ");
  }
  Serial.println();
  Serial.print("Sensor low: ");
  for (int a = 0; a < 8; a++) {
    sensorLow[a] = lowSens[a].average();
    Serial.print(sensorLow[a]);
    Serial.print("  ");
  }
  Serial.println();
  Serial.print("Calibration values:  ");
  for (int a = 0; a < 8; a++) {
    calibrationValues[a] = sensorLow[a] + (sensorHigh[a] - sensorLow[a]) / 2;
    Serial.print(calibrationValues[a]);
    Serial.print("  ");
  }
  Serial.println();
}

void transformBinary(int sensor[], int calib[]) {
  Serial.print("Sensor binary: ");
  for (int a = 0; a < 8; a++) {
    sensor[a] = (sensor[a] > calib[a]) ? 0 : 1;
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
/*
  Statistic();    // constructor
  void clear();   // reset all counters
  void add(double); // add a new value
  long count();   // # values added
  double sum();   // total
  double minimum(); // minimum
  double maximum(); // maximum
  double average(); // average
  double pop_stdev(); // population std deviation
  double unbiased_stdev();  // unbiased std deviation
*/
