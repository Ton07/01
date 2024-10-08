#define TRIG_PIN 9       // กำหนดขา TRIG ของ HC-SR04
#define ECHO_PIN 10      // กำหนดขา ECHO ของ HC-SR04
#define RELAY1_PIN 3     // กำหนดขารีเลย์ 1 (GPIO3)
#define RELAY2_PIN 4     // กำหนดขารีเลย์ 2
#define SPEAKER_PIN 5    // กำหนดขาลำโพง
#define THRESHOLD_DISTANCE 10  // ระยะที่ต้องการตรวจสอบ (10 cm)

bool isBelowThreshold = false; // สถานะเพื่อตรวจสอบการข้ามขีดจำกัด
bool hasOperated = false; // สถานะบอกว่าเงื่อนไขได้ถูกทำงานไปแล้ว

void setup() {
  Serial.begin(9600);  // เริ่มต้นการสื่อสารกับ Serial Monitor
  pinMode(TRIG_PIN, OUTPUT);  // กำหนดขา TRIG เป็นเอาต์พุต
  pinMode(ECHO_PIN, INPUT);   // กำหนดขา ECHO เป็นอินพุต
  pinMode(RELAY1_PIN, OUTPUT); // กำหนดขารีเลย์ 1 เป็นเอาต์พุต
  pinMode(RELAY2_PIN, OUTPUT); // กำหนดขารีเลย์ 2 เป็นเอาต์พุต
  pinMode(SPEAKER_PIN, OUTPUT); // กำหนดขาลำโพงเป็นเอาต์พุต
  digitalWrite(RELAY1_PIN, LOW); // ตั้งค่ารีเลย์ 1 ให้ปิดเมื่อเริ่มต้น
  digitalWrite(RELAY2_PIN, LOW); // ตั้งค่ารีเลย์ 2 ให้ปิดเมื่อเริ่มต้น
}

void loop() {
  // ตรวจสอบระยะจากเซ็นเซอร์ HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  // แสดงค่าระยะ
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // เงื่อนไขเมื่อระยะต่ำกว่า 10 cm และยังไม่เคยทำงานในรอบนี้
  if (distance <= THRESHOLD_DISTANCE && !hasOperated) {
    Serial.println("Distance below threshold, operating relay 1 and speaker.");
    digitalWrite(RELAY1_PIN, HIGH);  // เปิดรีเลย์ 1
    tone(SPEAKER_PIN, 1000);         // ลำโพงดังที่ความถี่ 1000 Hz
    delay(3000);                     // รอ 3 วินาที
    noTone(SPEAKER_PIN);             // ปิดเสียงลำโพง
    digitalWrite(RELAY1_PIN, LOW);   // ปิดรีเลย์ 1

    hasOperated = true;              // ตั้งค่าว่าเงื่อนไขถูกทำงานแล้ว
  }

  // รีเซ็ตสถานะเมื่อระยะมากกว่า 10 cm
  if (distance > THRESHOLD_DISTANCE) {
    hasOperated = false;             // รีเซ็ตสถานะการทำงาน
  }

  // ตรวจสอบว่ามีข้อมูลเข้ามาจาก Serial Monitor หรือไม่
  if (Serial.available() > 0) {
    char input = Serial.read();  // อ่านค่าจาก Serial Monitor

    // ก่อนที่รีเลย์ 2 จะทำงาน ตัดการทำงานของรีเลย์ 1
    digitalWrite(RELAY1_PIN, LOW);  // ปิดรีเลย์ 1

    // ตรวจสอบว่ากดหมายเลขใด
    if (input == '1') {
      runRelay2(5); // ทำงานเป็นเวลา 15 วินาที
    } 
    else if (input == '2') {
      runRelay2(10); // ทำงานเป็นเวลา 30 วินาที
    } 
    else if (input == '3') {
      runRelay2(3); // ทำงานเป็นเวลา 45 วินาที
    }
  }

  delay(500); // หน่วงเวลาเพื่อความเสถียรของระบบ
}

// ฟังก์ชันสำหรับควบคุมรีเลย์ 2 พร้อมกับการนับถอยหลัง
void runRelay2(int durationInSeconds) {
  digitalWrite(RELAY2_PIN, HIGH);  // เปิดรีเลย์ 2
  Serial.print("Relay 2 operating for ");
  Serial.print(durationInSeconds);
  Serial.println(" seconds");

  // แสดงการนับถอยหลัง
  for (int i = durationInSeconds; i > 0; i--) {
    Serial.print("Time remaining: ");
    Serial.print(i);
    Serial.println(" seconds");
    delay(1000);  // รอ 1 วินาที
  }

  digitalWrite(RELAY2_PIN, LOW);  // ปิดรีเลย์ 2
  Serial.println("Relay 2 stopped.");
}