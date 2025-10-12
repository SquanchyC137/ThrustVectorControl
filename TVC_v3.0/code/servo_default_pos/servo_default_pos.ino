#include <Servo.h>

Servo servo1;
Servo servo2;


int pos1 = 120;
int pos2 = 45;

void setup() {
  servo1.attach(9);
  servo2.attach(10);
  servo1.write(pos1);
  servo2.write(pos2);
}

void loop() {
  // servo1.write(pos1);
  // servo2.write(pos2);
  // delay(1000);

  // servo1.write(pos1 + 45);
  // servo2.write(pos2 + 45);
  // delay(100);

  // servo1.write(pos1);
  // servo2.write(pos2);
  // delay(100);

  // servo1.write(pos1 - 45);
  // servo2.write(pos2 - 45);
  // delay(100);
}








