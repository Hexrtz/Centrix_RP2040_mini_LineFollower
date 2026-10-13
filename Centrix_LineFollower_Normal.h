#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

// ============================ HARDWARE CONFIG ==============================
extern uint8_t M1_IN1;   // Left Motor IN1 (default 0)
extern uint8_t M1_IN2;   // Left Motor IN2 (default 1)
extern uint8_t M2_IN1;   // Right Motor IN1 (default 2)
extern uint8_t M2_IN2;   // Right Motor IN2 (default 3)

extern uint8_t SW1;      // Switch 1 (default 4)
extern uint8_t SW2;      // Switch 2 (default 5)
extern uint8_t LED_PIN;  // RGB NeoPixel (default 16)

extern uint8_t MUX_SIG;  // Mux Analog SIG (default GP26 / A0)
extern uint8_t MUX_S[4]; // S0, S1, S2, S3 (default {11, 10, 9, 8})

const int NUM_SENSORS = 16;

// ฟังก์ชันปรับเปลี่ยน Pin เซนเซอร์ และ มอเตอร์ สำหรับ User
void setSensorPins(uint8_t sig, uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3);
void setMotorPins(uint8_t m1_in1, uint8_t m1_in2, uint8_t m2_in1, uint8_t m2_in2);

// ============================ RUNTIME CONFIG ===============================
extern bool LINE_IS_BLACK;    // true = เส้นดำพื้นขาว, false = เส้นขาวพื้นดำ
extern bool SENSOR_REVERSE;   // true if CH0 is on the RIGHT side
extern bool LEFT_INVERT;      // flip if left motor spins backwards
extern bool RIGHT_INVERT;     // flip if right motor spins backwards

extern uint32_t LOOP_US;      // PD period 1000 us (1 kHz)
extern int CAL_SPEED;         // spin speed while calibrating (%)
extern int NOISE_TH;          // ignore normalized values below this (0..1000)
extern int LINE_TH;           // max value below this = no line
extern int MUX_SETTLE_US;     // mux settle time per channel
extern int MOTOR_DEADBAND;    // PWM deadband compensation
extern int TURN_SPEED;        // ความเร็วในการเลี้ยว 90 องศา (%)
extern int CROSS_FORWARD_MS;  // เวลาเดินหน้าเล็กน้อยก่อนเลี้ยว (ms)
extern int CROSS_PASS_MS;     // เวลาเดินหน้าข้ามแยกกรณี action 'p' (ms)

extern const uint32_t PWM_FREQ;
extern const int PWM_RANGE;

// ============================ EEPROM =======================================
const uint32_t CAL_MAGIC = 0x4C465031;  // "LFP1"
struct CalData {
  uint32_t magic;
  uint16_t minV[NUM_SENSORS];
  uint16_t maxV[NUM_SENSORS];
  uint32_t checksum;
};

// ============================ GLOBALS ======================================
extern Adafruit_NeoPixel led;

extern uint16_t rawVal[NUM_SENSORS];
extern uint16_t MIN[NUM_SENSORS], MAX[NUM_SENSORS];
#define calMin MIN
#define calMax MAX

extern int normVal[NUM_SENSORS];
extern bool calibrated;
extern bool onLine;

extern float lastError;
extern float lastPosition;
extern uint32_t lastPDus;

// ============================ API FUNCTIONS ================================

// 1. System & Initialization
void initRobot();
void waitForStart();
void checkEmergencyStop();

// 2. Motor Controls
void Motor(float left, float right);
void Stop();
void driveRaw(uint8_t in1, uint8_t in2, float pct);

// 3. Sensors & Calibration
void readRaw();
float position();
bool readPosition(float &pos);
void Calibrate();
bool loadCalibration();
void saveCalibration();
void printCalibration();
void ShowSensor();
void ShowLine();

// 4. Line Follower (PD)
void PD(float speed, float Kp, float Kd);
void PD_Time(float speed, float Kp, float Kd, uint32_t time_ms);
void PD_Cross(float speed, float Kp, float Kd, char action = 's');
void PD_Left(float speed, float Kp, float Kd, char action = 's');
void PD_Right(float speed, float Kp, float Kd, char action = 's');
void handleAction(char action, float speed = 0);
void turnLeft();
void turnRight();

// 5. Buttons
bool pressed(uint8_t pin);

// 6. LED & Visual Feedback
void setLed(uint8_t r, uint8_t g, uint8_t b);
void blinkLed(uint8_t r, uint8_t g, uint8_t b, int times, int ms);
void LedGreen();
void LedRed();
void LedBlue();
void rainbowUpdate();
