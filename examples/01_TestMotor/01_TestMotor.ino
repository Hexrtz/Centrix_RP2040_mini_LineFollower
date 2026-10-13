/*
 * ============================================================================
 *  Centrix_LineFollower_Normal - 01_TestMotor
 *  ตัวอย่างการทดสอบมอเตอร์ (เดินหน้า, ถอยหลัง, เลี้ยว, สปิน, เบรก)
 * ============================================================================
 */
#include <Centrix_LineFollower_Normal.h>

void setup() {
  initRobot();  // เริ่มต้นบอร์ดและมอเตอร์
  Serial.println(F("=== Test Motor Starting in 2 seconds ==="));
  delay(2000);

  // 1. เดินหน้าตรง ความเร็ว 50% เป็นเวลา 1.5 วินาที
  Serial.println(F("Forward 50%"));
  Motor(50, 50);
  delay(1500);

  // 2. เบรกหยุด 1 วินาที
  Serial.println(F("Stop"));
  Stop();
  delay(1000);

  // 3. ถอยหลัง ความเร็ว 40% เป็นเวลา 1.5 วินาที
  Serial.println(F("Backward 40%"));
  Motor(-40, -40);
  delay(1500);
  Stop();
  delay(1000);

  // 4. เลี้ยวซ้าย (ล้อซ้าย 20%, ล้อขวา 50%) เป็นเวลา 1.5 วินาที
  Serial.println(F("Turn Left"));
  Motor(20, 50);
  delay(1500);
  Stop();
  delay(1000);

  // 5. เลี้ยวขวา (ล้อซ้าย 50%, ล้อขวา 20%) เป็นเวลา 1.5 วินาที
  Serial.println(F("Turn Right"));
  Motor(50, 20);
  delay(1500);
  Stop();
  delay(1000);

  // 6. หมุนสปินอยู่กับที่ไปทางซ้าย (Spin Left)
  Serial.println(F("Spin Left"));
  turnLeft();
  delay(1000);

  // 7. หมุนสปินอยู่กับที่ไปทางขวา (Spin Right)
  Serial.println(F("Spin Right"));
  turnRight();
  delay(1000);

  Serial.println(F("=== Test Motor Completed ==="));
}

void loop() {
  // ไม่ต้องใส่โค้ดใน loop
  Stop();
}
