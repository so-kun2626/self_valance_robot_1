#include<Arduino.h>
#include<ESP32Encoder.h>
#include<Wire.h>
#include<math.h>

const int MUP_addr = 0x68;

const int ENCODER_A_PIN = 34;
const int ENCODER_B_PIN = 35;
const int motorPin1 = 25;
const int motorPin2 = 26;
const int motorPin3 = 13;
const int motorPin4 = 27;

const float WHEEL_RADIUS = 0.024;
const float GIAR_RATIO = 30.0;
const float WHEEL_PULSES = 7*2*GIAR_RATIO;
const float WHEEL_CIRCUMFERENCE = 2.0*PI*WHEEL_RADIUS;
const float DISTANSE_PER_PULSE = WHEEL_CIRCUMFERENCE / WHEEL_PULSES;


struct IMU_data{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;
};

IMU_data sensorData;

unsigned long lastTime = 0.0; //時間の初期化
float filteredAngle = 0.0;  //フィルタリングされた角度の初期化
int16_t lastCount = 0;  //
unsigned long lastTime = 0;
float filteredSpeed = 0.0;

// PID制御のパラメータ
float Kp = 26.7; //比例ゲイン
float Ki = 0.5;  //積分ゲイン
float Kd = 0.8;  //微分ゲイン

float targetAngle = -3.2; //目標角度（水平状態）
float lastError = 0.0; //前回の誤差
float integral = 0.0; //積分項の初期値

const unsigned long CONTROL_PERIOD = 10000; //制御周期（マイクロ秒）

ESP32Encoder encoder1; //エンコーダーオブジェクトの作成 

float calculateAngle(int16_t accX, int16_t accZ){
    return atan2(accX, accZ)*180/M_PI;  // atan2関数を使用して角度を計算し、度に変換
}

//モーター用の関数
void driveMotor(int power){
    if(power > -20 && power < 20){
        // 死点を考慮して、出力が小さい場合はモーターを止める
        analogWrite(motorPin1, 0);
        analogWrite(motorPin2, 0);
        analogWrite(motorPin3, 0);
        analogWrite(motorPin4, 0);
        return;
    }

    int offset = 10; // モーターの死点を補正するためのオフセット値

    if(power > 0){
        power = power + offset;
    }
    else if(power < 0){
        power = power - offset;
    }

    power = constrain(power, -255, 255);

    if(power > 0){
        analogWrite(motorPin1, power);
        analogWrite(motorPin2, 0);
        analogWrite(motorPin3, power);
        analogWrite(motorPin4, 0);
    }
    else if(power < 0){
        analogWrite(motorPin1, 0);
        analogWrite(motorPin2, power);
        analogWrite(motorPin3, 0);
        analogWrite(motorPin4, power);
    }
}



