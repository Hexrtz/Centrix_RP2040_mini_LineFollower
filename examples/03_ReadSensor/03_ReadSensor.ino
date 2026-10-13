/*
 * ============================================================================
 *  Centrix_LineFollower_Normal - 03_ReadSensor
 *  ตัวอย่างการอ่านค่าเซนเซอร์อนาล็อก 16 ช่อง, อ่านตำแหน่งเส้น (position),
 *  และวิธีปรับเปลี่ยน Pin SIG / MUX S0-S3 สำหรับ User
 * ============================================================================
 */
#include <Centrix_LineFollower_Normal.h>

void setup() {
  // -------------------------------------------------------------------------
  // [หมายเหตุ] การปรับแต่งขาเซนเซอร์ (หากใช้ขาบอร์ดไม่เหมือนค่าเริ่มต้น):
  // ค่าเริ่มต้นคือ: SIG=26(A0), S0=11, S1=10, S2=9, S3=8
  // สามารถกำหนดขาใหม่ได้ด้วยคำสั่ง:
  // setSensorPins(sig, s0, s1, s2, s3);
  // -------------------------------------------------------------------------
  setSensorPins(26, 11, 10, 9, 8);  // กำหนดขาเซนเซอร์ (SIG, S0, S1, S2, S3)

  initRobot();  // เริ่มต้นระบบ

  Serial.println(F("=== Sensor Reading Demo ==="));
  Serial.println(F("Place the robot on/off the line and observe values."));
  delay(1000);

  // เลือกว่าจะทดสอบแบบไหน (ปลดคอมเมนต์ฟังก์ชันที่ต้องการดู):
  // ShowSensor();  // แสดงค่า ADC ดิบ 16 ช่อง (0..4095)
  // ShowLine();    // แสดงค่า Calibrated (0..1000) และตำแหน่งเส้น (-100..+100%)
}

void loop() {
  // อ่านค่าตำแหน่งเส้น (คืนค่า 0..15000, โดยที่ 7500 คือจุดกึ่งกลาง)
  float pos = position();

  // แสดงผลค่าตำแหน่งเส้น และสถานะว่าจับเส้นได้หรือไม่
  Serial.print(F("Position: "));
  Serial.print(pos);

  Serial.print(F(" | onLine: "));
  if (onLine) {
    Serial.print(F("YES (Line Detected)"));
    LedGreen();
  } else {
    Serial.print(F("NO  (Line Lost)"));
    LedRed();
  }

  // พิมพ์ค่าดิบเซนเซอร์ตัวริมซ้าย (CH0) และริมขวา (CH15)
  Serial.print(F(" | Left[0]: "));
  Serial.print(normVal[0]);
  Serial.print(F(" | Right[15]: "));
  Serial.println(normVal[15]);

  delay(50);
}
