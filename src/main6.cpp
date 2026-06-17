//リポバッテリー用のコードです
#include<Arduino.h>
#include<Wire.h>
#include<math.h>
#include<ESP32Encoder.h>

const int MPU_addr = 0x68;

const int motorPin1 = 13; // Motor 1 control pin
const int motorPin2 = 27; // Motor 2 control pin
const int motorPin3 = 25; // Motor 3 control pin
const int motorPin4 = 26; // Motor 4 control pin

const int ENCODER_A_R = 32;
const int ENCODER_B_R = 33;
const int ENCODER_A_L = 34;
const int ENCODER_B_L = 35;

struct IMU_data{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;
};

IMU_data sensorData;

ESP32Encoder encoderR;
ESP32Encoder encoderL;

const float WHEEL_RADIUS = 0.024;
const float GIA_RATIO = 30;
const float WHEEL_PULSE = 14*GIA_RATIO;
const float WHEEL_CIRCUMFERENCE = 2.0*PI*WHEEL_RADIUS;
const float DISTANCE_PER_PULSE = WHEEL_CIRCUMFERENCE/WHEEL_PULSE;

unsigned long lastTime = 0.0; // For timing the loop
float filteredAngle = 0.0;  // For complementary filter

int64_t lastCountR = 0;
int64_t lastCountL = 0;
float filteredSpeed = 0.0;

// PID control parameters
float Kp =27.8;  // Proportional gain　
float Ki = 0.00;  // Integral gain
float Kd = 0.76;  // Derivative gain

float Kp_Speed =9.0;
float Ki_speed = 0.9;
float speedIntegral = 0.0;

float mechanicalOffset = -3.0;  // 目標角度（水平状態）
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

    int offset = 3; // モーターの死点を補正するためのオフセット値

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

    ESP32Encoder::useInternalWeakPullResistors = puType::up;
    encoderL.attachHalfQuad(ENCODER_A_L, ENCODER_B_L);
    encoderR.attachHalfQuad(ENCODER_A_R, ENCODER_B_R);
    encoderL.setCount(0);
    encoderR.setCount(0);

    Serial.println("AccAngle,FilteredAngle,FilteredSpeed");

    lastTime = micros();  // タイミングの初期化
}

void loop(){
    unsigned long currentTime = micros();
    unsigned long timeDiff = currentTime - lastTime;
    
    if(timeDiff >= 10000){ // 10ms（0.01秒）ごとに計算
        // --- 時間の計算 ---
        float dt = timeDiff / 1000000.0; // マイクロ秒を秒に変換

        // 三半規管（MPU6050）から角度を聞く
        Wire.beginTransmission(MPU_addr);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom((uint8_t)MPU_addr, (size_t)14, (bool)true);

        sensorData.accX = Wire.read() << 8 |  Wire.read();
        sensorData.accY = Wire.read() << 8 |  Wire.read();
        sensorData.accZ = Wire.read() << 8 |  Wire.read();
        Wire.read(); Wire.read(); // 温度データをスキップ
        sensorData.gyroX = Wire.read() << 8 |  Wire.read();
        sensorData.gyroY = Wire.read() << 8 |  Wire.read();
        sensorData.gyroZ = Wire.read() << 8 |  Wire.read();

        // 角度と角速度を計算
        float accAngle = calculateAngle(sensorData.accX, sensorData.accZ);
        float gyroRate = -sensorData.gyroY / 131.0; 

        // コンプリメンタリフィルタで滑らかな角度（filteredAngle）を推定
        filteredAngle = 0.98 * (filteredAngle + gyroRate * dt) + 0.02 * accAngle;

        // エンコーダーからスピードを聞く
        int64_t currentCountL = encoderL.getCount();
        int64_t currentCountR = encoderR.getCount();
        
        int64_t countDiffL = currentCountL - lastCountL;
        int64_t countDiffR = currentCountR - lastCountR;

        // 左右それぞれのスピードを計算 (m/s)
        float rawSpeedL = (countDiffL * DISTANCE_PER_PULSE) / dt;
        float rawSpeedR = (countDiffR * DISTANCE_PER_PULSE) / dt;

        // 左右のスピードの「平均」を出す！
        float rawSpeed_Avg = (rawSpeedL + rawSpeedR) / 2.0;
        
        // ローパスフィルタでガタガタを滑らかにする
        filteredSpeed = 0.8 * filteredSpeed + 0.2 * rawSpeed_Avg;


        //  速度PIDの仕事：目標角度を決める
        float targetSpeed = 0.0; // 最終目標は「止まること」！
        float speedError = targetSpeed - filteredSpeed;

        speedIntegral += speedError*dt;

        speedIntegral = constrain(speedIntegral, -5.0, 5.0);

        // スピードのズレから「どれくらい傾くべきか」をP制御で計算
        float speedOutput = -(Kp_Speed * speedError) - (Ki_speed * speedIntegral);

        // いままで手動で入れていた「重心のゲタ合わせ」
        

        // 最終的な目標角度（無茶な指示にならないよう ±10度で制限！）
        float targetAngle = constrain(speedOutput + mechanicalOffset, -10.0, 10.0);


        // 角度PID：モーターのパワーを決める
        // 常に変わる社長の指示（targetAngle）に向かってバランスをとる！
        float error = targetAngle - filteredAngle; 
        integral += error * dt; 
        float derivative = (error - lastError) / dt; 
        lastError = error; // 誤差を保存

        // 職人のPID出力計算
        float output = Kp * error + Ki * integral + Kd * derivative;

        // 筋肉（モーター）を動かす
        driveMotor((int)output);


       
        // 記憶の更新とプロッタへの出力
        lastCountL = currentCountL;
        lastCountR = currentCountR;
        lastTime = currentTime;

        Serial.print(targetAngle);
        Serial.print(",");
        Serial.print(filteredAngle);
        Serial.print(",");
        Serial.println(filteredSpeed * 10); // スピードは波形を見やすくするために10倍しておくよ
    }
}