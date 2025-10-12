#include <Servo.h>

Servo servo1;
Servo servo2;


int pos1 = 45;
int pos2 = 120;
int initiate = 0;

void setup() {
servo1.attach(10);
servo2.attach(9);
}

void loop() {

  servo1.write(pos1);
  servo2.write(pos2);

  if(initiate = 0){
    initfct();}
  else{
    south();
  }
  west();
  north();
  east();
  delay(50);
}

void initfct(){
  for(int i = 0; i<=20; i++){
    servo2.write(pos2 + i);
    initiate = 1;
    delay(20);
  }
}

void south(){
  for(int i = 0; i<=20; i++){
    servo2.write(pos2 + i);
    servo1.write(pos1 -20 + i);
    delay(20);
  }
}

void west(){
  for(int i = 0; i<=20; i++){
    servo1.write(pos1 + i);
    servo2.write(pos2 +20 - i);
    delay(20); 
 } 
}

void north(){
  for(int i = 0; i <= 20; i++){
    servo2.write(pos2 -i);
    servo1.write(pos1 +20 -i);
    delay(20);
  }
}

void east(){
  for(int i = 0; i <=20; i++){
    servo1.write(pos1 - i);
    servo2.write(pos2 -20 +i);
    delay(20);
  }  
}
