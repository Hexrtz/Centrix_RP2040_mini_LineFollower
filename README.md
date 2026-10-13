# Centrix_LineFollower_Normal Library

ไลบรารี Arduino ภาษา C++ สำหรับหุ่นยนต์เดินตามเส้นที่ใช้ไมโครคอนโทรลเลอร์ **RP2040-Zero**
ออกแบบในสไตล์ **Direct API** เพื่อให้เขียนโค้ดได้ง่ายที่สุด ไม่ต้องประกาศ Object หรือพิมพ์คำสั่งยาวๆ

---

## วิธีติดตั้งเข้ากับ Arduino IDE

### วิธีที่ 1: คัดลอกโฟลเดอร์ไปยัง `Arduino/libraries`
1. คัดลอกโฟลเดอร์ `Centrix_LineFollower_Normal` นี้ทั้งโฟลเดอร์
2. นำไปวางไว้ในโฟลเดอร์ไลบรารีของ Arduino:
   - **macOS / Linux:** `~/Documents/Arduino/libraries/`
   - **Windows:** `Documents\Arduino\libraries\`
3. ปิดแล้วเปิด Arduino IDE ใหม่อีกครั้ง
4. สามารถเรียกใช้ได้ทันทีจากเมนู: **File > Examples > Centrix_LineFollower_Normal > LineFollow_Simple**

### วิธีที่ 2: วางไฟล์ไว้ในโฟลเดอร์ Sketch โดยตรง
นำไฟล์ `Centrix_LineFollower_Normal.h` และ `Centrix_LineFollower_Normal.cpp` ไปวางไว้ในโฟลเดอร์เดียวกันกับไฟล์ `.ino` ของคุณ แล้วเรียกใช้งาน:
```cpp
#include "Centrix_LineFollower_Normal.h"
```

---

## ตัวอย่างการใช้งาน (Quick Start)

```cpp
#include <Centrix_LineFollower_Normal.h>

void setup() {
  initRobot();       // เริ่มต้นบอร์ด, มอเตอร์, เซนเซอร์ และโหลดค่า Calibrate
  waitForStart();    // แสดงไฟ Rainbow + รอสวิตช์ SW2 ออกตัว (หรือกด SW1 เพื่อ Calibrate)

  // วิ่งตามเวลา (Speed%, Kp, Kd, เวลา ms)
  PD_Time(60, 1.3, 12, 8000);
}

void loop() {
  // วิ่งในลูป
  PD(65, 1.3, 18);
}
```

## ตัวอย่างโปรแกรมใน Examples (7 หัวข้อพร้อมใช้งาน)
ในเมนู **File > Examples > Centrix_LineFollower_Normal** มีตัวอย่างโค้ดแยกตามหัวข้อดังนี้:
1. **`01_TestMotor`** : ทดสอบเดินหน้า, ถอยหลัง, เลี้ยวซ้าย, เลี้ยวขวา, หมุนสปิน, เบรก
2. **`02_TestLED`** : ทดสอบไฟ RGB NeoPixel (สีพื้นฐาน, กระพริบ, ไฟสถานะ, ไฟสีรุ้ง Rainbow)
3. **`03_ReadSensor`** : อ่านค่าดิบ ADC 16 ช่อง, อ่านตำแหน่งเส้น (position), และวิธีกำหนด Pin เซนเซอร์
4. **`04_Calibration_EEPROM`** : การ Calibrate แสง ขาว/ดำ, บันทึก/โหลด EEPROM, และแสดงตารางค่าแสง
5. **`05_SimplePD`** : ตัวอย่างการเดินตามเส้นด้วย PD แบบเข้าใจง่าย พร้อมคำแนะนำการจูน Kp, Kd
6. **`06_FullControl_Mission`** : ตัวอย่างการวิ่งภารกิจเต็มรูปแบบ (Soft Start, ตรวจจับแยกซ้าย/ขวา, เลี้ยว 90 องศา, ข้ามแยก, หยุดเส้นชัย)
7. **`07_Competition_Template`** : เทมเพลตลงสนามแข่งจริง (ตั้งค่า Pin เซนเซอร์ได้, มีโหมดตรวจเช็คเซนเซอร์, รวมรูปแบบ PD หลากหลายในแต่ละ Mission, และใช้ Stop() ใน loop)

---

## รายการฟังก์ชัน API ทั้งหมด

| หมวดหมู่ | ฟังก์ชัน | คำอธิบาย |
| :--- | :--- | :--- |
| **เริ่มต้นระบบ & กำหนด Pin** | `initRobot()` | ตั้งค่า Pin, PWM 20kHz, ADC 12-bit, LED, โหลดค่า EEPROM |
| | `setSensorPins(sig, s0, s1, s2, s3)` | กำหนด Pin เซนเซอร์ (SIG, S0..S3) หากใช้ขาต่างจากค่าเริ่มต้น |
| | `setMotorPins(m1_in1, m1_in2, m2_in1, m2_in2)` | กำหนด Pin มอเตอร์ซ้าย/ขวา |
| | `waitForStart()` | แสดงไฟ Rainbow และรอปุ่ม SW2 ออกตัว (กด SW1 เพื่อ Calibrate) |
| | `checkEmergencyStop()` | ตรวจสอบการกดปุ่ม SW1 หรือ SW2 ขณะวิ่งเพื่อหยุดฉุกเฉิน |
| **มอเตอร์** | `Motor(left, right)` | ขับมอเตอร์เปอร์เซ็นต์ (-100 ถึง 100 %) |
| | `Stop()` | เบรกมอเตอร์ทั้งสองข้าง |
| | `driveRaw(in1, in2, pct)` | สั่งขับมอเตอร์ตรงแบบเปอร์เซ็นต์ |
| **เซนเซอร์** | `position()` | อ่านตำแหน่งเส้น คืนค่า `0 .. 15000` (กึ่งกลาง = `7500`) |
| | `readPosition(pos)` | อ่านตำแหน่งเส้นและคืนค่า `true/false` ตามสถานะเจอเส้น |
| | `onLine` | ตัวแปร `bool` บ่งบอกว่าหุ่นยนต์จับเส้นได้หรือไม่ |
| | `Calibrate()` | สั่งหมุนตัวกวาดเส้นเพื่อ Calibrate และบันทึก EEPROM |
| | `ShowSensor()` | วนพิมพ์ค่า ADC ดิบ 16 ช่องออก Serial Monitor |
| | `ShowLine()` | วนพิมพ์ค่า Calibrated และตำแหน่งเส้นออก Serial Monitor |
| **เดินตามเส้น** | `PD(speed, Kp, Kd)` | เดินตามเส้น 1 สเต็ปที่ความถี่คงที่ 1 kHz |
| | `PD_Time(speed, Kp, Kd, time_ms)` | เดินตามเส้นเป็นเวลาที่กำหนด (มิลลิวินาที) |
| | `PD_Cross(speed, Kp, Kd, action)` | วิ่งจนริมซ้ายและขวาเจอเส้นตัดพร้อมกัน แล้วทำ action (`'s'`, `'l'`, `'r'`, `'p'`) |
| | `PD_Left(speed, Kp, Kd, action)` | วิ่งจนริมซ้ายเจอเส้นตัวเดียว แล้วทำ action (`'s'`, `'l'`, `'r'`, `'p'`) |
| | `PD_Right(speed, Kp, Kd, action)` | วิ่งจนริมขวาเจอเส้นตัวเดียว แล้วทำ action (`'s'`, `'l'`, `'r'`, `'p'`) |
| | `turnLeft()` | สั่งหมุนเลี้ยวซ้าย 90 องศาจนเซนเซอร์กลางจับเส้นใหม่ |
| | `turnRight()` | สั่งหมุนเลี้ยวขวา 90 องศาจนเซนเซอร์กลางจับเส้นใหม่ |
| **ปุ่ม & ไฟ LED** | `pressed(pin)` | ตรวจสอบการกดปุ่ม (SW1, SW2) พร้อม Debounce |
| | `setLed(r, g, b)` | เปลี่ยนสีไฟ RGB |
| | `blinkLed(r, g, b, count, ms)` | สั่งกระพริบไฟ RGB |
| | `LedGreen()`, `LedRed()`, `LedBlue()` | สั่งติดไฟสีเขียว, แดง, น้ำเงิน |
| | `rainbowUpdate()` | อัปเดตสีไฟ Rainbow (เรียกในลูป) |
