#include<Arduino.h>
#include<Wire.h>
#include<math.h>

const int MPU_addr = 0x68;

struct IMU_data{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;
};

IMU_data sensorData;

float calculateAngle(int16_t accX, int16_t accZ){
  return atan2 (accX, accZ) * 180 / M_PI;
}

void setup(){
  Serial.begin(115200);
  Wire.begin();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("accX,accY,accZ,gyroX,gyroY,gyroZ,angle");
}

void loop(){

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  // call overload with exact types to avoid ISO C++ ambiguous overloads
  Wire.requestFrom((uint8_t)MPU_addr, (size_t)14, (bool)true);

  sensorData.accX = Wire.read() << 8 |  Wire.read();
  sensorData.accY = Wire.read() << 8 |  Wire.read();
  sensorData.accZ = Wire.read() << 8 |  Wire.read();
  sensorData.gyroX = Wire.read() << 8 |  Wire.read();
  sensorData.gyroY = Wire.read() << 8 |  Wire.read();
  sensorData.gyroZ = Wire.read() << 8 |  Wire.read();

  float angle = calculateAngle(sensorData.accX, sensorData.accZ);

  Serial.print(sensorData.accX);
  Serial.print(",");
  Serial.print(sensorData.accY);
  Serial.print(",");
  Serial.print(sensorData.accZ);
  Serial.print(",");
  Serial.print(sensorData.gyroX);
  Serial.print(",");
  Serial.print(sensorData.gyroY);
  Serial.print(",");
  Serial.print(sensorData.gyroZ);
  Serial.print(",");
  Serial.println(angle);

  delay(100);
}
