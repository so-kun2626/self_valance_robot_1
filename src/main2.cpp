#include<Arduino.h>
#include<Wire.h>

const int MPU_addr = 0x68;

struct IMU_data{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;
};

IMU_data sensorData;

void setup(){
  Serial.begin(115200);
  Wire.begin();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("MPU6050 is ready");
}

void loop(){

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);

  sensorData.accX = Wire.read() << 8 |  Wire.read();
  sensorData.accY = Wire.read() << 8 |  Wire.read();
  sensorData.accZ = Wire.read() << 8 |  Wire.read();
  sensorData.gyroX = Wire.read() << 8 |  Wire.read();
  sensorData.gyroY = Wire.read() << 8 |  Wire.read();
  sensorData.gyroZ = Wire.read() << 8 |  Wire.read();

  Serial.print("accX: ");
  Serial.println(sensorData.accX);
  Serial.print("accY: ");
  Serial.println(sensorData.accY);
  Serial.print("accZ: ");
  Serial.println(sensorData.accZ);
  Serial.print("gyroX: ");
  Serial.println(sensorData.gyroX);
  Serial.print("gyroY: ");
  Serial.println(sensorData.gyroY);
  Serial.print("gyroZ: ");
  Serial.println(sensorData.gyroZ);

  delay(100);
}
