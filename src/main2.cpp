#include<Arduino.h>
#include<Wire.h>
#include<math.h>

const int MPU_addr = 0x68;

struct IMU_data{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;
};

IMU_data sensorData;

unsigned long lastTime = 0; // For timing the loop
float filteredAngle = 0;  // For complementary filter

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

  Serial.println("AccAngle,FilteredAngle");
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

  Wire.read(); Wire.read(); // Skip two bytes
  
  sensorData.gyroX = Wire.read() << 8 |  Wire.read();
  sensorData.gyroY = Wire.read() << 8 |  Wire.read();
  sensorData.gyroZ = Wire.read() << 8 |  Wire.read();

  float accAngle = calculateAngle(sensorData.accX, sensorData.accZ);

  unsigned long currentTIme = micros();
  float dt = (currentTIme - lastTime) / 1000000.0; // Convert to seconds
  lastTime = currentTIme;

  // Complementary filter
  filteredAngle = 0.98 * (filteredAngle + sensorData.gyroY * dt / 131.0) + 0.02 * accAngle;  // 今回のセンサーの感度は131 LSB/°/sなので、gyroYを131で割って角速度を得る
  Serial.print(accAngle);
  Serial.print(",");
  Serial.println(filteredAngle);

  delay(10); // Loop every 10ms
}
