/*
 * ============================================================================
 *  Centrix_LineFollower_Normal - 02_TestLED
 *  ตัวอย่างการทดสอบไฟ RGB NeoPixel (สีพื้นฐาน, กระพริบ, Rainbow)
 * ============================================================================
 */
#include <Centrix_LineFollower_Normal.h>

void setup() {
  initRobot();
  Serial.println(F("=== Test RGB LED ==="));

  // 1. ทดสอบสีพื้นฐาน (เขียว, แดง, น้ำเงิน)
  Serial.println(F("LED Green"));
  LedGreen();
  delay(1000);

  Serial.println(F("LED Red"));
  LedRed();
  delay(1000);

  Serial.println(F("LED Blue"));
  LedBlue();
  delay(1000);

  // 2. สั่งสีอิสระด้วย setLed(R, G, B) [0..255]
  Serial.println(F("Custom Color: Yellow"));
  setLed(255, 255, 0);  // เหลือง
  delay(1000);

  Serial.println(F("Custom Color: Cyan"));
  setLed(0, 255, 255);  // ฟ้า
  delay(1000);

  Serial.println(F("Custom Color: Magenta"));
  setLed(255, 0, 255);  // ม่วง
  delay(1000);

  // 3. กระพริบไฟ blinkLed(R, G, B, จำนวนครั้ง, เวลา ms)
  Serial.println(F("Blink Blue 3 times"));
  blinkLed(0, 0, 255, 3, 200);

  Serial.println(F("=== Entering Rainbow Loop ==="));
}

void loop() {
  // 4. วนแสดงไฟสีรุ้ง Rainbow แบบต่อเนื่อง
  rainbowUpdate();
  delay(10);
}
