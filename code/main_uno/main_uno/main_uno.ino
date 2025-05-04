// #include <Servo.h>
#include <ESP32Servo.h>

Servo myServo; // Servo-Objekt erstellen

void setup() {
  myServo.attach(2); // Servo mit Pin 2 verbinden
  myServo.write(113);
}

void loop() {
  // Servo zu 0 Grad bewegen
  // myServo.write(0);
  // delay(1000); // 1 Sekunde warten

  // // Servo zu 90 Grad bewegen
  // myServo.write(5);
  // delay(1000); // 1 Sekunde warten

  // // Servo zu 180 Grad bewegen
  // myServo.write(10);
  // delay(1000); // 1 Sekunde warten
}
