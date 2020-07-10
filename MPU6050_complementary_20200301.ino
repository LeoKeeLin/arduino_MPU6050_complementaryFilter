#include <Wire.h>

float gForceX, gForceY, gForceZ;
float rotX, rotY, rotZ;

float rotXError = 0;
float rotYError = 0;
float rotZError = 0;

float accX, accY;
float rad_to_deg = 180/3.141592654;
float roll_angle = 0;
float pitch_angle = 0;
float yaw_angle = 0;

float prevTime, time, elapsedTime;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupMPU();
  gyroError();
  time = millis();
}

void loop() {
  prevTime = time;
  time = millis();
  elapsedTime = (time - prevTime) / 1000;
  recordAccelRegisters();
  recordGyroRegisters();
  printData();
}

void setupMPU(){
  Wire.beginTransmission(0b1101000);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1B);
  Wire.write(0x00000000);                       // gyro +/- 250 deg/s
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1C);
  Wire.write(0b00000000);                       // accel +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6);
  while(Wire.available() < 6);
  gForceX = (Wire.read()<<8|Wire.read()) / 16384.0;
  gForceY = (Wire.read()<<8|Wire.read()) / 16384.0;
  gForceZ = (Wire.read()<<8|Wire.read()) / 16384.0;
}

void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6);
  while(Wire.available() < 6);
  rotX = (Wire.read()<<8|Wire.read()) / 131.0;
  rotY = (Wire.read()<<8|Wire.read()) / 131.0;
  rotZ = (Wire.read()<<8|Wire.read()) / 131.0;
}

void gyroError() {
  for (int i = 0; i < 200; i++)
  {
    recordGyroRegisters();
    rotXError += rotX;
    rotYError += rotY;
    rotZError += rotZ;
  }
  rotXError /= 200;
  rotYError /= 200;
  rotZError /= 200;
}

void printData() {
  rotX -= rotXError;
  rotY -= rotYError;
  rotZ -= rotZError;
  accX = (atan((gForceY)/sqrt(pow((gForceX),2) + pow((gForceZ),2)))*rad_to_deg);
  accY = (atan(-1*(gForceX)/sqrt(pow((gForceY),2) + pow((gForceZ),2)))*rad_to_deg);
  roll_angle = 0.02 * accX + 0.98 * (rotX * elapsedTime + roll_angle / 2);
  pitch_angle = 0.02 * accY + 0.98 * (rotY * elapsedTime + pitch_angle / 2);
  yaw_angle += rotZ * elapsedTime;

  /*Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);*/
  /*Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);*/
  
  Serial.print("roll_angle : ");
  Serial.print(roll_angle);
  Serial.print("\t\tpitch_angle : ");
  Serial.print(pitch_angle);
  Serial.print("\t\tyaw_angle : ");
  Serial.println(yaw_angle);

  /*Serial.print("\t\troll_angle : ");
  Serial.print(roll_angle * rad_to_deg);
  Serial.print("\t\tpitch_angle : ");
  Serial.println(pitch_angle * rad_to_deg);*/
}
