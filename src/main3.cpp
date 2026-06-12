#include<Arduino.h>
#include<Wire.h>
#include<math.h>

const int MPU_addr = 0x68;

const int motorPin1 = 13; // Motor 1 control pin
const int motorPin2 = 27; // Motor 2 control pin
const int motorPin3 = 25; // Motor 3 control pin
const int motorPin4 = 26; // Motor 4 control pin

struct IMU_data{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;
};

IMU_data sensorData;

unsigned long lastTime = 0.0

\

; // For timing the loop
float filteredAngle = 0.0;  // For complementary filter

// PID control parameters
float Kp =26.7;  // Proportional gain　
float Ki = 0.5 ;  // Integral gain
float Kd = 0.8;  // Derivative gain

float targetAngle = -3.2;  // 目標角度（水平状態）
float lastError =0.0;     // 前回の誤差
float integral = 0.0;  // 積分項の初期値

const unsigned long CONTROL_PERIOD = 10000; // 制御周期

float calculateAngle(int16_t accX, int16_t accZ){
    return atan2(accX, accZ)*180 / M_PI;  // atan2関数を使用して角度を計算し、度に変換
}

void driveMotor(int power){

    if(power > -20&& power < 20){ 
        // 死点を考慮して、出力が小さい場合はモーターを止める
        analogWrite(motorPin1, 0);
        analogWrite(motorPin2, 0);
        analogWrite(motorPin3, 0);
        analogWrite(motorPin4, 0);
        return;
    }

    int offset = 10; // モーターの死点を補正するためのオフセット値

    if(power > 0){
        power = power + offset; // 正の値にオフセットを適用
    }
    else if(power < 0){
        power = power - offset; // 負の値にオフセットを適用
    }

    power = constrain(power, -255, 255); // パワーを-255から255の範囲に制限

    if(power > 0){
        //前に倒れたら前進
        analogWrite(motorPin1, power);
        analogWrite(motorPin2, 0);
        analogWrite(motorPin3, power);
        analogWrite(motorPin4, 0);
    }
    else if(power < 0){
        //後ろに倒れたら後進
        analogWrite(motorPin1, 0);
        analogWrite(motorPin2, -power);
        analogWrite(motorPin3, 0);
        analogWrite(motorPin4, -power);
    }
   
}

void setup(){
    Serial.begin(115200);
    Wire.begin();

    // モーター制御ピンを出力モードに設定
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);

    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);

    Serial.println("AccAngle,FilteredAngle");

    lastTime = micros();  // タイミングの初期化
}

void loop(){
    while(micros() - lastTime < CONTROL_PERIOD){
        // 制御周期が経過するまで待機
    }

    // MPU6050からセンサーデータを読み取る
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_addr, (size_t)14, (bool)true);

    sensorData.accX = Wire.read() << 8 |  Wire.read();
    sensorData.accY = Wire.read() << 8 |  Wire.read();
    sensorData.accZ = Wire.read() << 8 |  Wire.read();
    Wire.read(); Wire.read(); // Skip two bytes (温度データをスキップ)
    sensorData.gyroX = Wire.read() << 8 |  Wire.read();
    sensorData.gyroY = Wire.read() << 8 |  Wire.read();
    sensorData.gyroZ = Wire.read() << 8 |  Wire.read();

    // 加速度センサーから角度を計算
    float accAngle = calculateAngle(sensorData.accX, sensorData.accZ);
    // ジャイロセンサーから角速度を計算
    float gyroRate = -sensorData.gyroY / 131.0; // ジャイロの感度は131 LSB/°/s
    // タイミングの計算
    unsigned long currentTime = micros();
    float dt = (currentTime - lastTime) / 1000000.0; // 秒に変換

    lastTime = micros(); // タイミングの更新

    // コンプリメンタリフィルタで角度を推定
    filteredAngle = 0.98 * (filteredAngle + gyroRate * dt) + 0.02 * accAngle;

    // PID制御の計算
    float error = targetAngle - filteredAngle; // 目標角度と現在の角度の誤差
    integral += error * dt; // 積分項の更新
    float derivative = (error - lastError) / dt; // 微分項の計算
    lastError = error; // 誤差を保存

    // PID出力の計算
    float output = Kp * error + Ki * integral + Kd * derivative;

    // モーター駆動
    driveMotor((int)output);

    // デバッグ用に角度とPID出力をシリアルモニタに表示
    Serial.print(filteredAngle);
    Serial.print(",");
    Serial.println(output);
}