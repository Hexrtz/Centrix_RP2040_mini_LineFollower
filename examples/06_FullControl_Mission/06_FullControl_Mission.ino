/*
 * ============================================================================
 *  Centrix_LineFollower_Normal - 06_FullControl_Mission
 *  ตัวอย่าง Full Control การควบคุมหุ่นยนต์วิ่งตามเส้นสำหรับลงสนามแข่งขันจริง:
 *    - ออกตัวนุ่มนวล (Soft Start) ด้วยเวลา (PD_Time)
 *    - วิ่งจนเจอทางแยกซ้าย แล้วเลี้ยวซ้าย (PD_Left)
 *    - วิ่งจนเจอทางแยกขวา แล้วเลี้ยวขวา (PD_Right)
 *    - วิ่งข้ามแยกตัดแรกโดยไม่เลี้ยว (PD_Cross ด้วย action 'p')
 *    - วิ่งจนเจอเส้นหยุดปลายทาง แล้วเบรกหยุด (PD_Cross ด้วย action 's')
 * ============================================================================
 */
#include <Centrix_LineFollower_Normal.h>

void setup() {
  initRobot();       // เริ่มต้นบอร์ด, มอเตอร์, เซนเซอร์ 16 ช่อง, โหลดค่า EEPROM
  waitForStart();    // แสดงไฟ Rainbow + รอสวิตช์ SW2 ออกตัว (กด SW1 เพื่อ Calibrate)

  // ==================== ลำดับภารกิจ (MISSION SEQUENCE) ====================

  // 1. ออกตัวช่วงแรกด้วยความเร็ว 55% เป็นเวลา 1.5 วินาที
  Serial.println(F("[Step 1] Soft Start 1500 ms"));
  PD_Time(55, 1.2, 14, 1500);

  // 2. วิ่งต่อเนื่องจนเซนเซอร์ริมซ้ายเจอเส้นตัวเดียว แล้วเลี้ยวซ้าย 90 องศา ('l')
  Serial.println(F("[Step 2] Follow line until Left Junction -> Turn Left"));
  PD_Left(60, 1.3, 16, 'l');

  // 3. วิ่งต่อเนื่องจนเซนเซอร์ริมขวาเจอเส้นตัวเดียว แล้วเลี้ยวขวา 90 องศา ('r')
  Serial.println(F("[Step 3] Follow line until Right Junction -> Turn Right"));
  PD_Right(60, 1.3, 16, 'r');

  // 4. วิ่งทางตรงยาวด้วยความเร็ว 75% เป็นเวลา 2.5 วินาที
  Serial.println(F("[Step 4] Fast section 2500 ms"));
  PD_Time(75, 1.5, 22, 2500);

  // 5. วิ่งไปเรื่อยๆ จนเจอทางแยกตัดรูปบวก (+) ให้วิ่งข้ามแยกไปตรงๆ ('p' = pass)
  Serial.println(F("[Step 5] Cross Intersection -> Pass"));
  PD_Cross(60, 1.3, 16, 'p');

  // 6. วิ่งต่อไปจนถึงเส้นหยุดสุดท้าย (ริมซ้ายและขวาเจอเส้นพร้อมกัน) แล้วเบรกนิ่ง ('s')
  Serial.println(F("[Step 6] Finish Line -> Stop"));
  PD_Cross(60, 1.3, 16, 's');

  // ==================== สิ้นสุดภารกิจ ====================
  Stop();
  Serial.println(F("[Mission Complete!]"));
  blinkLed(0, 255, 0, 5, 150);  // กระพริบไฟเขียวฉลองจบภารกิจ
  while (1) {
    Stop();
    delay(500);
  }
}

void loop() {
  // เมื่อจบภารกิจใน setup() แล้ว loop() จะไม่ต้องทำงาน
}
