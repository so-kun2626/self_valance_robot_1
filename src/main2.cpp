#include<Arduino.h>
#include<wire.h>

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
  Wire.requestFrom(MPU_addr,2,true);

  sensorData.accX = Wire.read() << 8 |  Wire.read();

  Serial.print("accX: ");
  Serial.println(sensorData.accX);
  
  delay(100);
}
