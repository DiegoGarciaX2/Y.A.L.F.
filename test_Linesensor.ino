#include <Wire.h>
#define uchar unsigned char
uchar t;
//void send_data(short a1,short b1,short c1,short d1,short e1,short f1);
uchar data[16];
void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  t = 0;
}
void loop()
{
  int sensor[8];
  int x=0;
  Wire.requestFrom(9, 16);    // request 16 bytes from slave device #9
  while (Wire.available())   // slave may send less than requested
  {
    data[t] = Wire.read(); // receive a byte as character
    data[t]=(data[t]>70)?1:0;
    if(t%2==0){
      sensor[x]=data[t];
      x++;
    }
    if (t < 15){
      t++;
    }
    else{
      t = 0;
      x = 0;
    }
  }
  Serial.print("Sensor: ");
  for(x=0; x<8; x++){
    Serial.print(" ");
    Serial.print(sensor[x]);
  }
  Serial.println();
  int k=kvalue(sensor);
  Serial.print("k:");
  Serial.print(k);
  Serial.println();
  delay(2000);
}

int kvalue(int sensor[]){
  int control=0;
  int k=0;
  for(int x=0; x<=3 && control==0; x++){
    if(sensor[x]==0){
      control=1;
      k=x-3;
    }
  }
  for(int x=7; x>=4 && control==0; x--){
    if(sensor[x]==0){
      control=1;
      k=x-4;
    }
  }
  return k;
}
  
