#include <SCServo.h>

SMS_STS st;   // левая
SMS_STS st2;  // правая

#define S_RX_PIN  16
#define S_TX_PIN  17
#define S2_RX_PIN 4
#define S2_TX_PIN 5
#define SERVO_ID  1

const int PIN_A = 19;  // аналог PB13
const int PIN_B = 20;  // аналог PB14
const int PIN_C = 21;  // аналог PB15

// Позиции секторов — меняйте здесь
#define POS_LEFT_L   0   // LEFT серва, левый сектор
#define POS_LEFT_C   2048   // LEFT серва, центр
#define POS_LEFT_R   4095   // LEFT серва, правый сектор

#define POS_RIGHT_L  4095   // RIGHT серва, левый сектор
#define POS_RIGHT_C  2048   // RIGHT серва, центр
#define POS_RIGHT_R  0   // RIGHT серва, правый сектор

#define DEATHPOINT 1800
#define MAXPOINT 4095

#define MAX_SPEED 3000
#define SLOW_SPEED	300
#define ACC   50
int SPEED = MAX_SPEED;
uint8_t last_mode = 255;

void moveLeft(int pos){
  moveLeft(pos,MAX_SPEED);
}
void moveLeft(int pos, int speed) {
  st.WritePosEx(SERVO_ID, pos, speed, ACC);
  Serial.print("LEFT -> ");
  Serial.println(pos);
}

void moveRight(int pos) {
  moveRight(pos,MAX_SPEED);
}
void moveRight(int pos, int speed) {
  int reversePos = 4095 - pos; //Servo reversed
  st2.WritePosEx(SERVO_ID, reversePos, speed, ACC);
  Serial.print("RIGHT -> ");
  Serial.println(pos);
}

void handleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd.startsWith("LEFT=")) {
    int val = cmd.substring(5).toInt();
    if (val >= 0 && val <= 4095) moveLeft(val);
    else Serial.println("ERR: 0-4095");
  }
  else if (cmd.startsWith("RIGHT=")) {
    int val = cmd.substring(6).toInt();
    if (val >= 0 && val <= 4095) moveRight(val);
    else Serial.println("ERR: 0-4095");
  }

  // Оба сразу: BOTH=2048
  else if (cmd.startsWith("BOTH=")) {
    int val = cmd.substring(5).toInt();
    if (val >= 0 && val <= 4095) { moveLeft(val); moveRight(val); }
    else Serial.println("ERR: 0-4095");
  }
  else if (cmd.startsWith("SPEED=")){
    int val = cmd.substring(6).toInt();
    SPEED = val;
    Serial.print("New speed = ");
    Serial.println(SPEED);
  }else if (cmd=="TOP"){
    move_func_up();
  } else if (cmd=="DOWN"){
    move_func_down();
  } else if (cmd == "UPDOWN") {
  move_func_up_down();
} else if (cmd == "FAST") {
  move_func_fast();
} else if (cmd == "SLOW") {
  move_func_slow();
}
  else {
    Serial.println("Command: LEFT=N, RIGHT=N, BOTH=N");
  }
}
//OLD COMMANDS
void move_func_up_down() {
  moveLeft(MAXPOINT); moveRight(MAXPOINT);  // вверх
  delay(1500);

  moveLeft(DEATHPOINT + (MAXPOINT - DEATHPOINT) * 300 / 1024);
  moveRight(DEATHPOINT + (MAXPOINT - DEATHPOINT) * 300 / 1024);
  delay(200);

  moveLeft(MAXPOINT); moveRight(MAXPOINT);
  delay(200);

  moveLeft(DEATHPOINT + (MAXPOINT - DEATHPOINT) * 300 / 1024);
  moveRight(DEATHPOINT + (MAXPOINT - DEATHPOINT) * 300 / 1024);
  delay(200);

  moveLeft(MAXPOINT); moveRight(MAXPOINT);
  delay(200);

  moveLeft(DEATHPOINT + (MAXPOINT - DEATHPOINT) * 300 / 1024);
  moveRight(DEATHPOINT + (MAXPOINT - DEATHPOINT) * 300 / 1024);
  delay(500);

  moveLeft(DEATHPOINT); moveRight(DEATHPOINT);  // вниз
}

void move_func_up()
{
	moveLeft(MAXPOINT);//, MAX_SPEED
	moveRight(MAXPOINT);
}
void move_func_down()
{
	moveLeft(DEATHPOINT);//, MAX_SPEED
	moveRight(DEATHPOINT);
}
void move_func_fast() {
  #define P(x) (MAXPOINT - (MAXPOINT - DEATHPOINT) * (x) / 1024)

  moveLeft(MAXPOINT); moveRight(MAXPOINT);
  delay(2000);

  moveLeft(P(200)); delay(200);
  moveRight(P(200)); delay(300);

  moveLeft(P(400)); delay(200);
  moveRight(P(400)); delay(100);

  moveLeft(P(200)); delay(200);
  moveRight(P(200)); delay(100);

  moveLeft(P(400)); delay(200);
  moveRight(P(400)); delay(100);

  moveLeft(P(300)); delay(200);
  moveRight(P(300)); delay(100);

  moveLeft(P(100)); delay(100);
  moveRight(P(100)); delay(100);

  moveLeft(P(500)); moveRight(P(500)); delay(1500);
  moveLeft(P(200)); moveRight(P(200)); delay(150);

  moveLeft(P(500)); moveRight(P(500)); delay(200);
  moveLeft(P(200)); moveRight(P(200)); delay(1000);

  moveLeft(DEATHPOINT); delay(100);
  moveRight(DEATHPOINT);

  Serial.println("DONE");
  #undef P
}

void move_func_slow() {
  #define P(x) (MAXPOINT - (MAXPOINT - DEATHPOINT) * (x) / 1024)

  moveLeft(MAXPOINT, SLOW_SPEED); moveRight(MAXPOINT, SLOW_SPEED);  // 0, 1024
  delay(4500);

  moveLeft(P(300)); moveRight(P(700)); delay(200);
  moveLeft(MAXPOINT); moveRight(MAXPOINT); delay(200);

  moveLeft(P(300)); moveRight(P(700)); delay(200);
  moveLeft(MAXPOINT); moveRight(MAXPOINT); delay(1000);

  moveLeft(P(500)); moveRight(P(500)); delay(200);

  moveLeft(P(300)); moveRight(P(700)); delay(1000);

  moveLeft(P(500)); moveRight(P(500)); delay(1000);

  moveLeft(MAXPOINT); moveRight(MAXPOINT); delay(700);

  moveLeft(DEATHPOINT, SLOW_SPEED); moveRight(DEATHPOINT, SLOW_SPEED);  // 1024, 0

  #undef P
}

void (*doll_move[])() = {
  move_func_down,
  move_func_up,
  move_func_fast,
  move_func_slow,
  move_func_up_down
};

void setup() {
  pinMode(PIN_A, INPUT_PULLUP);
pinMode(PIN_B, INPUT_PULLUP);
pinMode(PIN_C, INPUT_PULLUP);
  Serial.begin(115200);
  delay(1000);

  Serial1.begin(1000000, SERIAL_8N1, S_RX_PIN, S_TX_PIN);
  st.pSerial = &Serial1;

  Serial2.begin(1000000, SERIAL_8N1, S2_RX_PIN, S2_TX_PIN);
  st2.pSerial = &Serial2;

  st.writeByte(SERVO_ID, 33, 0);
  st2.writeByte(SERVO_ID, 33, 0);
  delay(100);

  int ping = st.Ping(SERVO_ID);
  Serial.println(ping == -1 ? "LEFT: No response" : "LEFT: OK");

  int ping2 = st2.Ping(SERVO_ID);
  Serial.println(ping2 == -1 ? "RIGHT: No response" : "RIGHT: OK");
  Serial.println("Centred");
  moveLeft(2048);moveRight(2048);
  Serial.println("Ready. Command: LEFT=2000, RIGHT=1000, BOTH=2048");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    handleCommand(cmd);
  }
  uint8_t mode = 7
              - digitalRead(PIN_A)
              - (digitalRead(PIN_B) << 1)
              - (digitalRead(PIN_C) << 2);

if (mode <= 4 && mode != last_mode) {
  last_mode = mode;
  Serial.printf("Mode=%d\n", mode);
  doll_move[mode]();
}

delay(50);
}