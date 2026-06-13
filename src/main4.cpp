#include<Arduino.h>
#include<ESP32Encoder.h>

const int ENCODER_A_PIN = 34;
const int ENCODER_B_PIN = 35;

ESP32Encoder encoder1;

const float WHEEL_RADIUS = 0.024; // ホイールの半径（メートル）
const float GIAR_RATIO = 30.0; // ギア比（必要に応じて変更）
const float WHEEL_PULSES = 7*2*GIAR_RATIO; // ホイール1回転あたりのパルス数（エンコーダーの仕様とギア比に基づいて計算）
// ホイールの円周（メートル）
const float WHEEL_CIRCUMFERENCE = 2.0 * PI * WHEEL_RADIUS; 
 // 1パルスあたりの距離（メートル）
const float DISTANCE_PER_PULSE = WHEEL_CIRCUMFERENCE / WHEEL_PULSES;

// 速度計算のための変数
int64_t lastCount = 0;
unsigned long lastTime = 0;
float filteredSpeed = 0.0; // フィルタリングされた速度

void setup(){
    Serial.begin(115200);
 
    // エンコーダーの初期化
    ESP32Encoder::useInternalWeakPullResistors = puType::up; // 内部プルアップ抵抗を使用
    encoder1.attachHalfQuad(ENCODER_A_PIN, ENCODER_B_PIN); // ハーフクワッド接続
    encoder1.setCount(0); // カウントを0にリセット

    lastTime = micros(); // 初回の時間を記録

    Serial.println("Encoder initialized. Starting count...");
    Serial.println("Speed (m/s), Filtered Speed (m/s)");
}

void loop(){

    //今の時間を取得
    unsigned long currentTime = micros();
    //エンコーダーのカウントを取得
    int64_t currentCount = encoder1.getCount();

    //時間差を計算
    unsigned long timeDiff = currentTime - lastTime;
    //カウント差を計算
    int64_t countDiff = currentCount - lastCount;
    //速度を計算（メートル/秒）
    float speed = 0.0;
    if (timeDiff > 0) { // 時間差が0でないことを確認
        speed = (countDiff * DISTANCE_PER_PULSE) / (timeDiff / 1000000.0); // timeDiffを秒に変換
    }

    filteredSpeed = 0.8 * filteredSpeed + 0.2 * speed; // ローパスフィルタで速度を平滑化

    //速度をシリアルモニタに表示
    Serial.print(speed);
    Serial.print(",");
    Serial.println(filteredSpeed); 

    //次のループのために現在のカウントと時間を保存
    lastCount = currentCount;
    lastTime = currentTime;

    delay(10); // 10ミリ秒ごとにカウントを表示
}