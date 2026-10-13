#include "Centrix_LineFollower_Normal.h"

// ============================ PIN MAPPINGS =================================
uint8_t M1_IN1   = 0;
uint8_t M1_IN2   = 1;
uint8_t M2_IN1   = 2;
uint8_t M2_IN2   = 3;

uint8_t SW1      = 4;
uint8_t SW2      = 5;
uint8_t LED_PIN  = 16;

uint8_t MUX_SIG  = 26;
uint8_t MUX_S[4] = { 11, 10, 9, 8 };

void setSensorPins(uint8_t sig, uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3) {
  MUX_SIG = sig;
  MUX_S[0] = s0;
  MUX_S[1] = s1;
  MUX_S[2] = s2;
  MUX_S[3] = s3;
  for (int i = 0; i < 4; i++) {
    pinMode(MUX_S[i], OUTPUT);
  }
}

void setMotorPins(uint8_t m1_in1, uint8_t m1_in2, uint8_t m2_in1, uint8_t m2_in2) {
  M1_IN1 = m1_in1;
  M1_IN2 = m1_in2;
  M2_IN1 = m2_in1;
  M2_IN2 = m2_in2;
  pinMode(M1_IN1, OUTPUT); pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT); pinMode(M2_IN2, OUTPUT);
}

// ============================ CONFIG VALUES ================================
bool LINE_IS_BLACK    = true;
bool SENSOR_REVERSE   = false;
bool LEFT_INVERT      = false;
bool RIGHT_INVERT     = false;

uint32_t LOOP_US      = 1000;    // 1 kHz
int CAL_SPEED         = 45;      // %
int NOISE_TH          = 60;      // 0..1000
int LINE_TH           = 500;     // 0..1000
int MUX_SETTLE_US     = 20;      // us
int MOTOR_DEADBAND    = 0;
int TURN_SPEED        = 50;      // % ความเร็วเลี้ยว
int CROSS_FORWARD_MS  = 120;     // ms เดินหน้าก่อนเลี้ยว
int CROSS_PASS_MS     = 180;     // ms เดินหน้าข้ามแยกสำหรับ action 'p'

const uint32_t PWM_FREQ  = 20000;
const int PWM_RANGE      = 255;

// ============================ GLOBALS ======================================
Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);

uint16_t rawVal[NUM_SENSORS] = { 0 };
uint16_t MIN[NUM_SENSORS]    = { 0 };
uint16_t MAX[NUM_SENSORS]    = { 0 };

int normVal[NUM_SENSORS]     = { 0 };
bool calibrated              = false;
bool onLine                  = false;

float lastError              = 0.0f;
float lastPosition           = 7500.0f;  // 0..15000 (7500 = กลาง)
uint32_t lastPDus            = 0;

// ============================ SYSTEM =======================================
void initRobot() {
  Serial.begin(115200);

  pinMode(M1_IN1, OUTPUT); pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT); pinMode(M2_IN2, OUTPUT);
  analogWriteFreq(PWM_FREQ);
  analogWriteRange(PWM_RANGE);
  Motor(0, 0);

  for (int i = 0; i < 4; i++) pinMode(MUX_S[i], OUTPUT);
  analogReadResolution(12);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

  led.begin();
  led.setBrightness(40);

  EEPROM.begin(256);
  delay(1000);  // ให้เวลา Serial Monitor เชื่อมต่อ

  if (loadCalibration()) {
    calibrated = true;
    Serial.println(F("[Centrix] Calibration loaded from EEPROM"));
    printCalibration();
    blinkLed(0, 255, 0, 2, 120);
  } else {
    Serial.println(F("[Centrix] No calibration in EEPROM - press SW1 to calibrate"));
    blinkLed(255, 120, 0, 2, 120);
  }
  setLed(0, 40, 0);
}

void waitForStart() {
  blinkLed(0, 0, 255, 3, 250);  // นับถอยหลังสีน้ำเงิน
  while (1) {
    rainbowUpdate();
    if (pressed(SW2)) break;
    else if (pressed(SW1)) Calibrate();
  }
  LedGreen();
}

void checkEmergencyStop() {
  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH) return;
  Stop();
  Serial.println(F("[Centrix] EMERGENCY STOP - press SW2 to restart"));
  while (digitalRead(SW1) == LOW || digitalRead(SW2) == LOW) delay(5);
  delay(50);
  while (true) {
    setLed(255, 0, 0);
    delay(200);
    setLed(0, 0, 0);
    delay(200);
    if (pressed(SW2)) rp2040.reboot();
  }
}

bool pressed(uint8_t pin) {
  if (digitalRead(pin) == HIGH) return false;
  delay(20);
  if (digitalRead(pin) == HIGH) return false;
  while (digitalRead(pin) == LOW) delay(5);
  delay(20);
  return true;
}

// ============================ MOTORS =======================================
void driveRaw(uint8_t in1, uint8_t in2, float pct) {
  int pwm = (int)lroundf(constrain(pct, -100.0f, 100.0f) * PWM_RANGE / 100.0f);
  if (pwm == 0) {
    analogWrite(in1, 0);
    analogWrite(in2, 0);
    return;
  }

  int duty = abs(pwm);
  if (MOTOR_DEADBAND > 0) duty = map(duty, 1, PWM_RANGE, MOTOR_DEADBAND, PWM_RANGE);

  // Slow decay: ด้านหนึ่ง HIGH, อีกด้าน Inverted PWM
  if (pwm > 0) {
    analogWrite(in1, PWM_RANGE);
    analogWrite(in2, PWM_RANGE - duty);
  } else {
    analogWrite(in1, PWM_RANGE - duty);
    analogWrite(in2, PWM_RANGE);
  }
}

void Motor(float left, float right) {
  driveRaw(M1_IN1, M1_IN2, LEFT_INVERT ? -left : left);
  driveRaw(M2_IN1, M2_IN2, RIGHT_INVERT ? -right : right);
}

void Stop() {
  analogWrite(M1_IN1, PWM_RANGE);
  analogWrite(M1_IN2, PWM_RANGE);
  analogWrite(M2_IN1, PWM_RANGE);
  analogWrite(M2_IN2, PWM_RANGE);
}

// ============================ SENSORS ======================================
void readRaw() {
  for (int ch = 0; ch < NUM_SENSORS; ch++) {
    for (int b = 0; b < 4; b++) digitalWrite(MUX_S[b], (ch >> b) & 1);
    delayMicroseconds(MUX_SETTLE_US);
    int idx = SENSOR_REVERSE ? (NUM_SENSORS - 1 - ch) : ch;
    rawVal[idx] = analogRead(MUX_SIG);  // 0..4095
  }
}

float position() {
  readRaw();

  long sumW = 0;
  long sumV = 0;
  int maxV = 0;

  for (int i = 0; i < NUM_SENSORS; i++) {
    if (MAX[i] <= MIN[i]) {
      normVal[i] = 0;
      continue;
    }

    int v = map(rawVal[i], MIN[i], MAX[i], 0, 1000);
    v = constrain(v, 0, 1000);
    if (v > 800) v = 1000;
    else if (v < 250) v = 0;
    if (!LINE_IS_BLACK) v = 1000 - v;
    normVal[i] = v;

    if (v > maxV) maxV = v;

    sumW += (long)v * (i * 1000);
    sumV += v;
  }

  if (maxV >= LINE_TH && sumV > 0) {
    onLine = true;
    lastPosition = (float)sumW / sumV;  // 0..15000 (7500 = กลาง)
  } else {
    onLine = false;
  }

  return lastPosition;
}

bool readPosition(float &pos) {
  pos = position();
  return onLine;
}

void ShowSensor() {
  while (1) {
    readRaw();
    for (int i = 0; i < NUM_SENSORS; i++) {
      Serial.print(rawVal[i]);
      Serial.print(" : ");
    }
    Serial.println();
    delay(20);
  }
}

void ShowLine() {
  while (1) {
    float pos = position();
    for (int i = 0; i < NUM_SENSORS; i++) Serial.printf("%5d", normVal[i]);
    if (onLine) Serial.printf("   FOUND pos %+.0f %%\n", (pos - 7500.0f) / 75.0f);
    else Serial.println(F("   LOST"));
    delay(50);
  }
}

// ============================ CALIBRATION ==================================
uint32_t calChecksum(const CalData &d) {
  uint32_t s = d.magic;
  for (int i = 0; i < NUM_SENSORS; i++) s = s * 31 + d.minV[i] * 7 + d.maxV[i];
  return s;
}

void saveCalibration() {
  CalData d;
  d.magic = CAL_MAGIC;
  memcpy(d.minV, MIN, sizeof(MIN));
  memcpy(d.maxV, MAX, sizeof(MAX));
  d.checksum = calChecksum(d);
  EEPROM.put(0, d);
  EEPROM.commit();
  Serial.println(F("[Centrix] Calibration saved to EEPROM"));
}

bool loadCalibration() {
  CalData d;
  EEPROM.get(0, d);
  if (d.magic != CAL_MAGIC || d.checksum != calChecksum(d)) return false;
  memcpy(MIN, d.minV, sizeof(MIN));
  memcpy(MAX, d.maxV, sizeof(MAX));
  return true;
}

void printCalibration() {
  Serial.println(F("CH   BLACK/MAX  WHITE/MIN  RANGE"));
  for (int i = 0; i < NUM_SENSORS; i++) {
    int range = MAX[i] - MIN[i];
    Serial.printf("%2d   %9u  %9u  %5d%s\n", i, MAX[i], MIN[i], range,
                  range < 300 ? "  <-- LOW" : "");
  }
}

void Calibrate() {
  Serial.println(F("[Centrix] Calibrating..."));
  setLed(0, 180, 180);

  for (int i = 0; i < NUM_SENSORS; i++) {
    MIN[i] = 4095;
    MAX[i] = 0;
  }

  const int dirs[4] = { 1, -1, -1, 1 };  // หมุน ขวา, ซ้าย, ซ้าย, ขวา
  for (int step = 0; step < 4; step++) {
    Motor(CAL_SPEED * dirs[step], -CAL_SPEED * dirs[step]);
    uint32_t t0 = millis();
    while (millis() - t0 < 450) {
      readRaw();
      for (int i = 0; i < NUM_SENSORS; i++) {
        if (rawVal[i] < MIN[i]) MIN[i] = rawVal[i];
        if (rawVal[i] > MAX[i]) MAX[i] = rawVal[i];
      }
    }
  }
  Stop();
  delay(150);
  Motor(0, 0);

  printCalibration();

  bool ok = true;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (MAX[i] - MIN[i] < 300) ok = false;
  }

  calibrated = true;
  saveCalibration();

  if (ok) blinkLed(0, 255, 0, 2, 120);
  else {
    Serial.println(F("[Centrix] WARNING: low sensor range"));
    blinkLed(255, 120, 0, 3, 150);
  }
  setLed(0, 40, 0);
}

// ============================ LINE FOLLOW (PD) =============================
void PD(float speed, float Kp, float Kd) {
  checkEmergencyStop();

  while (micros() - lastPDus < LOOP_US) {}
  lastPDus = micros();

  // จุดกึ่งกลางคือ 7500 (error -100..+100 %)
  float error = (position() - 7500.0f) / 75.0f;

  float correction = Kp * error + Kd * (error - lastError);
  lastError = error;

  Motor(speed + correction, speed - correction);
}

void PD_Time(float speed, float Kp, float Kd, uint32_t time_ms) {
  uint32_t t0 = millis();
  while (millis() - t0 < time_ms) PD(speed, Kp, Kd);
}

// ---------------------------- เลี้ยว & แอ็กชันหลังเจอแยก ------------------
void turnLeft() {
  if (CROSS_FORWARD_MS > 0) {
    Motor(CAL_SPEED, CAL_SPEED);
    delay(CROSS_FORWARD_MS);
  }
  Motor(-TURN_SPEED, TURN_SPEED);
  delay(120);  // หมุนพ้นเส้นเดิม

  uint32_t t0 = millis();
  while (millis() - t0 < 1200) {  // safety timeout 1.2s
    position();
    if (normVal[6] >= LINE_TH || normVal[7] >= LINE_TH || normVal[8] >= LINE_TH || normVal[9] >= LINE_TH) {
      break;
    }
  }
  Stop();
}

void turnRight() {
  if (CROSS_FORWARD_MS > 0) {
    Motor(CAL_SPEED, CAL_SPEED);
    delay(CROSS_FORWARD_MS);
  }
  Motor(TURN_SPEED, -TURN_SPEED);
  delay(120);  // หมุนพ้นเส้นเดิม

  uint32_t t0 = millis();
  while (millis() - t0 < 1200) {  // safety timeout 1.2s
    position();
    if (normVal[6] >= LINE_TH || normVal[7] >= LINE_TH || normVal[8] >= LINE_TH || normVal[9] >= LINE_TH) {
      break;
    }
  }
  Stop();
}

void handleAction(char action, float speed) {
  char act = tolower(action);
  if (act == 's') {
    Stop();
  } else if (act == 'p') {
    // วิ่งข้ามเส้นตัดเล็กน้อยแล้วหยุด
    Motor(speed > 0 ? speed : CAL_SPEED, speed > 0 ? speed : CAL_SPEED);
    delay(CROSS_PASS_MS);
    Stop();
  } else if (act == 'l') {
    turnLeft();
  } else if (act == 'r') {
    turnRight();
  } else {
    Stop();
  }
}

// 1. วิ่ง PD จนกว่าเซนเซอร์ริมซ้ายและริมขวาจะเจอเส้นพร้อมกัน (Cross / T-junction)
void PD_Cross(float speed, float Kp, float Kd, char action) {
  // หากเพิ่งออกจากเส้นตัดเดิม ให้วิ่งพ้นเส้นเดิมก่อน
  while (normVal[0] >= LINE_TH && normVal[NUM_SENSORS - 1] >= LINE_TH) {
    checkEmergencyStop();
    PD(speed, Kp, Kd);
  }

  // วิ่งจนกว่าริมซ้ายและขวาจะเจอเส้นพร้อมกัน
  while (true) {
    checkEmergencyStop();
    PD(speed, Kp, Kd);
    if (normVal[0] >= LINE_TH && normVal[NUM_SENSORS - 1] >= LINE_TH) {
      break;
    }
  }

  handleAction(action, speed);
}

// 2. วิ่ง PD จนกว่าเซนเซอร์ริมซ้ายจะเจอเส้นตัวเดียว (แยกซ้าย)
void PD_Left(float speed, float Kp, float Kd, char action) {
  // หากเซนเซอร์ซ้ายยังติดเส้นเดิม ให้วิ่งพ้นก่อน
  while (normVal[0] >= LINE_TH) {
    checkEmergencyStop();
    PD(speed, Kp, Kd);
  }

  // วิ่งจนกว่าริมซ้ายจะเจอเส้นตัวเดียว (ขวาไม่เจอ)
  while (true) {
    checkEmergencyStop();
    PD(speed, Kp, Kd);
    if (normVal[0] >= LINE_TH && normVal[NUM_SENSORS - 1] < LINE_TH) {
      break;
    }
  }

  handleAction(action, speed);
}

// 3. วิ่ง PD จนกว่าเซนเซอร์ริมขวาจะเจอเส้นตัวเดียว (แยกขวา)
void PD_Right(float speed, float Kp, float Kd, char action) {
  // หากเซนเซอร์ขวายังติดเส้นเดิม ให้วิ่งพ้นก่อน
  while (normVal[NUM_SENSORS - 1] >= LINE_TH) {
    checkEmergencyStop();
    PD(speed, Kp, Kd);
  }

  // วิ่งจนกว่าริมขวาจะเจอเส้นตัวเดียว (ซ้ายไม่เจอ)
  while (true) {
    checkEmergencyStop();
    PD(speed, Kp, Kd);
    if (normVal[NUM_SENSORS - 1] >= LINE_TH && normVal[0] < LINE_TH) {
      break;
    }
  }

  handleAction(action, speed);
}

// ============================ LED & UI =====================================
void setLed(uint8_t r, uint8_t g, uint8_t b) {
  led.setPixelColor(0, led.Color(r, g, b));
  led.show();
}

void blinkLed(uint8_t r, uint8_t g, uint8_t b, int times, int ms) {
  for (int i = 0; i < times; i++) {
    setLed(r, g, b);
    delay(ms);
    setLed(0, 0, 0);
    delay(ms);
  }
}

void LedGreen() { setLed(0, 255, 0); }
void LedRed()   { setLed(255, 0, 0); }
void LedBlue()  { setLed(0, 0, 255); }

void rainbowUpdate() {
  static uint32_t last = 0;
  static uint16_t hue = 0;
  if (millis() - last < 10) return;
  last = millis();
  led.setPixelColor(0, led.gamma32(led.ColorHSV(hue)));
  led.show();
  hue += 256;
}
