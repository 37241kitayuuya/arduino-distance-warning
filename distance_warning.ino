const int BUZZER_PIN = 10;  // ブザーピン
const int ECHO_PIN = 11;    // 反射して戻った音波を受けるピン
const int TRIG_PIN = 12;    // 音波を出すピン
const int LED_PIN = 13;     // LEDピン

const int TRIG_STABLE_TIME_US = 2;   // 超音波センサー安定化タイム
const int TRIG_SIGNAL_TIME_US = 10;  // 超音波測定タイム

const unsigned long ECHO_TIMEOUT_US = 30000;  // タイムアウト値
const int MEASURE_INTERVAL_MS = 300;          // 次の測定までの待ち時間

const int WARNING_DISTANCE_CM = 20;  // 警告ゾーン（距離）
const int DANGER_DISTANCE_CM = 10;   // 危険ゾーン（距離）

const int LED_BLINK_COUNT = 5;     // LEDの点滅回数
const int LED_BLINK_TIME_MS = 80;  // LEDの点灯・消灯の時間

// ブザーのタイプにより変更
const int BUZZER_ON = LOW;    // ブザーON
const int BUZZER_OFF = HIGH;  // ブザーOFF

const int BUZZER_SHORT_TIME_MS = 80;  // ブザーを鳴らす時間


void setup() {
  Serial.begin(9600);  // シリアルモニター使用

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // 最初はLEDとブザーをOFF
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, BUZZER_OFF);
}


void loop() {

  // 3回測定して平均距離を取得
  float distance = getAverageDistancecm();

  // 距離を判定して、LEDとブザーを制御する
  if (distance <= DANGER_DISTANCE_CM) {

    // 危険距離の場合
    // LEDを高速点滅し、ブザーを連続で鳴らす
    digitalWrite(BUZZER_PIN, BUZZER_ON);

    blankLed(LED_BLINK_COUNT);

  } else if (distance <= WARNING_DISTANCE_CM) {

    // 注意距離の場合
    // LEDを点灯し、ブザーを短く1回鳴らす
    digitalWrite(LED_PIN, HIGH);

    digitalWrite(BUZZER_PIN, BUZZER_ON);
    delay(BUZZER_SHORT_TIME_MS);

  } else {

    // 安全距離の場合
    // LEDとブザーをOFF
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, BUZZER_OFF);
  }

  // ブザーを止める
  digitalWrite(BUZZER_PIN, BUZZER_OFF);

  // シリアルモニターに距離を表示
  Serial.print("距離： ");
  Serial.print(distance);
  Serial.println(" cm");

  // 次の測定まで少し待つ
  delay(MEASURE_INTERVAL_MS);
}


// 距離を3回測定して平均値を返す関数（cm）
float getAverageDistancecm() {

  const int SAMPLE_COUNT = 3;       // 平均をとるための測定回数
  const int SAMPLE_INTERVAL_MS = 50; // 測定間隔

  float total = 0;  // 測定した距離の合計

  for (int i = 0; i < SAMPLE_COUNT; i++) {

    total += getDistancecm();

    delay(SAMPLE_INTERVAL_MS);
  }

  return total / SAMPLE_COUNT;
}


// 超音波センサーで距離を測って返す関数（cm）
float getDistancecm() {

  // 超音波センサーの安定化
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(TRIG_STABLE_TIME_US);

  // 測定開始
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(TRIG_SIGNAL_TIME_US);
  digitalWrite(TRIG_PIN, LOW);

  // ECHOがHIGHだった時間を計測
  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    ECHO_TIMEOUT_US
  );

  float distance;

  if (duration == 0) {

    // 反射が戻ってこなかった場合
    distance = 999.0;

  } else {

    // 往復時間 × 音速 ÷ 2 で距離を計算
    distance = duration * 0.0343 / 2;
  }

  return distance;
}


// LEDを指定された回数だけ点滅させる関数
void blankLed(int count) {

  for (int i = 0; i < count; i++) {

    // LED点灯
    digitalWrite(LED_PIN, HIGH);
    delay(LED_BLINK_TIME_MS);

    // LED消灯
    digitalWrite(LED_PIN, LOW);
    delay(LED_BLINK_TIME_MS);
  }
}
