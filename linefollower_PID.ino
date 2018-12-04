#include <Statistic.h>
#include <PID_v1.h>
#include <Wire.h>
#define uchar unsigned char
#define led 4
#define strSpeed 255
#define int1 6
#define int2 5
#define int3 11
#define int4 10
uchar t;
int calib[8];
uchar data[16];
double setpoint, vchange, pos, strspeed = 255;
double Kp = 0.25, Ki = 10, Kd = 5;
PID motor(&pos, &vchange, &setpoint, Kp, Ki, Kd, DIRECT);
Statistic highSens[8];
Statistic lowSens[8];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  calibrateSensors(calib);
  setpoint = 0;
  pos = 0;
  motor.SetMode(AUTOMATIC);
  t = 0;
  for (int a = 0; a < 8; a++) {
    highSens[a].clear();
    lowSens[a].clear();
  }
}
void loop() {
  int sensor[8];
  getSensorValues(sensor);
  pos = linePos(sensor, calib);
  Serial.print("Position value: ");
  Serial.print(pos);
  Serial.println();
  motor.Compute();
  Serial.print("Speed motor A:  ");
  Serial.print(strSpeed + vchange);
  Serial.println();
  Serial.print("Speed motor B:  ");
  Serial.print(strSpeed - vchange);
  Serial.println();
  motorSpeed();
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
  digitalWrite(led, HIGH);
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
  } while (rep < 100);

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

int linePos(int sensor[], int calib[]) {
  int sensorBinary[8];
  int k;
  Serial.print("Sensor binary: ");
  for (int a = 0; a < 8; a++) {
    sensorBinary[a] = (sensor[a] > calib[a]) ? 0 : 1;
    Serial.print(sensorBinary[a]);
    Serial.print("  ");
  }
  Serial.println();
  digitalWrite(led, LOW);
  for (int a = 0; a < 8; a++) {
    if (sensorBinary[a] == 0 && sensorBinary[a + 1] == 0) {
      k = a - 3;
    }
    else {
      k = (sensor[a - 1] > sensor[a + 1]) ? a - 2 : a - 3;
    }
  }
  return k;
}

void motorSpeed() {
  digitalWrite(int1, HIGH);
  digitalWrite(int2, strspeed - vchange);
  digitalWrite(int3, HIGH);
  digitalWrite(int4, strspeed + vchange);
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
