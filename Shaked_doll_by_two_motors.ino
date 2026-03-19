// Требует установки библиотеки SCServo (из репозитория waveshare/workloads/scservo)

#include <Arduino.h>
#include "SMS_STS.h"

// UART для левой сервы
#define S_RX_PIN 16
#define S_TX_PIN 17

// UART для правой сервы
#define S2_RX_PIN 4
#define S2_TX_PIN 5

SMS_STS st;   // левая
SMS_STS st2;  // правая

const uint8_t SERVO_ID = 1;

// Вспомогательная функция
void printReading(const char* prefix, const char* name, int val) {
  if (val < 0) Serial.printf("%s %s: ERR\n", prefix, name);
  else Serial.printf("%s %s: %d\n", prefix, name, val);
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(200);

  // UART1 — левая серва
  Serial1.begin(1000000, SERIAL_8N1, S_RX_PIN, S_TX_PIN);
  st.pSerial = &Serial1;

  // UART2 — правая серва
  Serial2.begin(1000000, SERIAL_8N1, S2_RX_PIN, S2_TX_PIN);
  st2.pSerial = &Serial2;

  Serial.println("=== Dual ST3020 torque test starting ===");

  // --- PING ---
  int ping = st.Ping(SERVO_ID);
  if (ping == -1) Serial.println("LEFT: No response");
  else Serial.printf("LEFT Servo ID %d OK\n", ping);

  ping = st2.Ping(SERVO_ID);
  if (ping == -1) Serial.println("RIGHT: No response");
  else Serial.printf("RIGHT Servo ID %d OK\n", ping);

  // --- Wheel mode ---
  if (st.WheelMode(SERVO_ID) != -1) Serial.println("LEFT set to WHEEL mode");
  else Serial.println("LEFT failed to set WHEEL mode");

  if (st2.WheelMode(SERVO_ID) != -1) Serial.println("RIGHT set to WHEEL mode");
  else Serial.println("RIGHT failed to set WHEEL mode");

  delay(100);

  // --- Torque ON ---
  if (st.EnableTorque(SERVO_ID, 1) != -1) Serial.println("LEFT torque ENABLED");
  else Serial.println("LEFT torque enable FAILED");

  if (st2.EnableTorque(SERVO_ID, 1) != -1) Serial.println("RIGHT torque ENABLED");
  else Serial.println("RIGHT torque enable FAILED");

  delay(200);
}

void loop() {
  static int16_t speeds[] = {
    500, 1000, 1500, 2000, 2500, 3000,
    0,
    -500, -1000, -1500, -2000, -2500, -3000,
    0
  };

  for (size_t i = 0; i < sizeof(speeds)/sizeof(speeds[0]); ++i) {
    int16_t sp = speeds[i];
    Serial.printf("\n=== Set speed = %d ===\n", sp);

    // Установка скорости на обе сервы
    int r1 = st.WriteSpe(SERVO_ID, sp, 0);
    int r2 = st2.WriteSpe(SERVO_ID, sp, 0);

    Serial.printf("LEFT WriteSpe:  %s\n", (r1 == -1 ? "ERR" : "OK"));
    Serial.printf("RIGHT WriteSpe: %s\n", (r2 == -1 ? "ERR" : "OK"));

    delay(800);

    // --- Чтение телеметрии ---
    int load1 = st.ReadLoad(SERVO_ID);
    int curr1 = st.ReadCurrent(SERVO_ID);
    int volt1 = st.ReadVoltage(SERVO_ID);
    int temp1 = st.ReadTemper(SERVO_ID);

    int load2 = st2.ReadLoad(SERVO_ID);
    int curr2 = st2.ReadCurrent(SERVO_ID);
    int volt2 = st2.ReadVoltage(SERVO_ID);
    int temp2 = st2.ReadTemper(SERVO_ID);

    printReading("LEFT",  "Load", load1);
    printReading("LEFT",  "Current", curr1);
    printReading("LEFT",  "Voltage", volt1);
    printReading("LEFT",  "Temp", temp1);

    printReading("RIGHT", "Load", load2);
    printReading("RIGHT", "Current", curr2);
    printReading("RIGHT", "Voltage", volt2);
    printReading("RIGHT", "Temp", temp2);

    delay(600);
  }

  // --- Остановка ---
  st.WriteSpe(SERVO_ID, 0, 0);
  st2.WriteSpe(SERVO_ID, 0, 0);
  delay(200);

  st.EnableTorque(SERVO_ID, 0);
  st2.EnableTorque(SERVO_ID, 0);

  Serial.println("Torque DISABLED. Test cycle finished.");

  while (true) delay(1000);
}
