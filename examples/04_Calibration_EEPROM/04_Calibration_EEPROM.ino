/*
 * ============================================================================
 *  Centrix_LineFollower_Normal - 04_Calibration_EEPROM
 *  ตัวอย่างการ Calibrate แสง, การบันทึก/โหลดค่าจาก EEPROM, และแสดงตารางค่าแสง
 * ============================================================================
 */
#include <Centrix_LineFollower_Normal.h>

void setup() {
  initRobot();  // จะพยายามโหลดค่า Calibrate จาก EEPROM อัตโนมัติ

  Serial.println(F("============================================"));
  Serial.println(F("  Sensor Calibration & EEPROM Demo"));
  Serial.println(F("  - กดปุ่ม SW1 เพื่อเริ่ม Calibrate หมุนตัวเก็บค่าแสง"));
  Serial.println(F("  - กดปุ่ม SW2 เพื่อข้ามและเริ่มแสดงผล"));
  Serial.println(F("============================================"));

  while (1) {
    rainbowUpdate();
    if (pressed(SW1)) {
      Serial.println(F("Starting Calibration..."));
      Calibrate();  // หุ่นยนต์จะหมุนตัว 4 สเต็ปเพื่อเก็บค่า Min/Max และบันทึกลง EEPROM
      Serial.println(F("Calibration Done & Saved!"));
      break;
    } else if (pressed(SW2)) {
      Serial.println(F("Using Loaded Calibration from EEPROM."));
      break;
    }
  }

  // แสดงตารางค่าแสง MIN (ขาว), MAX (ดำ), และ RANGE (ผลต่าง)
  printCalibration();

  LedGreen();
}

void loop() {
  // อ่านค่าตำแหน่งเส้นเพื่อทดสอบว่าคาลิเบรตได้แม่นยำหรือไม่
  float pos = position();
  Serial.printf("Line Position: %.1f | onLine: %s\n", pos, onLine ? "YES" : "NO");
  delay(100);
}
