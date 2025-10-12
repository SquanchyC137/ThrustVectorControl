#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

#define conval 0.0174533   /* rad->deg | 1 deg = pi/180 = 0.0174532 */

Adafruit_MPU6050 mpu;
Servo servo1;
Servo servo2;

const int delaytime_loop = 10; /* main refresh time */
const int test_time = 15; 
const int xy_corr_cd = 50; /* xy-correction refresh time */
const double pos1 = 120;   /* servo 1 default x-position */
const double pos2 = 45;    /* servo 2 default y-position */
double currX, currY = 0;
double gx, sum_gx, gy, sum_gy = 0;
int g_counter = 0;
unsigned long timeSTART, timeEND, timeDIFF, timeCOUNT = 0;

void servo_test(){
  // movement test x,y +-20degrees
  int i = 0;
  for(i = 1; i <= 20; i++){
    servo1.write(pos1-i);
    delay(test_time);
  }
  for(i = 1; i <= 40; i++){
    servo1.write(pos1-20+i);
    delay(test_time);
  }
  for(i = 1; i <= 20; i++){
    servo1.write(pos1+20-i);
    delay(test_time);
  }
  for(i = 1; i <= 20; i++){
    servo2.write(pos2+i);
    delay(test_time);
  }
  for(int i = 1; i <= 40; i++){
    servo2.write(pos2+20-i);
    delay(test_time);
  }
  for(int i = 1; i <= 20; i++){
    servo2.write(pos2-20+i);
    delay(test_time);
  }
}

void setup() {
  Serial.begin(9600);
  mpu.begin();

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  servo1.attach(9);  /* turns around x - axis */
  servo2.attach(10); /* turns around y - axis */
  /* !!!  X and Y Servo upside down -> movement needs to be inverted  !!! */

  /* set default servo position and values */
  servo1.write(pos1);
  servo2.write(pos2);
  currX = pos1;
  currY = pos2;

  servo_test();
}



double avg_g_pSec(double sum_g, int count_g, double time){
  return (sum_g * time) / (count_g * 1000);
}

void correctX(double currX){
  servo1.write(currX);
}

void correctY(double currY){
  servo2.write(currY);
}

double toleranceFilter(double g){
  if(abs(g) < 0.2){
    g = 0;
  }
  return g;
}


void loop() {

  /* eval time intervals to correct xy-positions */
  timeDIFF = timeEND - timeSTART;
  timeCOUNT += timeDIFF;
  if(timeCOUNT >= xy_corr_cd){
    // Serial.print("currX start: ");
    // Serial.println(currX);
    currX -= avg_g_pSec(sum_gx, g_counter, timeCOUNT);
    Serial.print("avg °/100ms: ");
    Serial.print(currX);
    Serial.print(" g_counter: ");
    Serial.print(g_counter);
    Serial.print(" sum_gx: ");
    Serial.println(sum_gx);
    Serial.print("currX end: ");
    Serial.println(currX);
    Serial.println();
    currY -= avg_g_pSec(sum_gy, g_counter, timeCOUNT);

    correctX(currX);
    correctY(currY);

    timeCOUNT -= xy_corr_cd;
    g_counter = 0;
    sum_gx = 0;
    sum_gy = 0;
  }

  // static bool _init = true;
  // if (_init){
  //   _init = false;
  // } else {timeEND = millis();}
  timeSTART = millis();


  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);


  gx = (g.gyro.x / conval)+3.12;
  gx = toleranceFilter(gx);
  sum_gx += gx;
  gy = (g.gyro.y / conval)-1.331;
  gy = toleranceFilter(gy);
  sum_gy += gy;
  g_counter += 1;




  // Serial.print("gx: ");
  // Serial.print(gx, 3);
  // Serial.print(", gy: ");
  // Serial.print(gy, 3);

  // Serial.print("\n");

  // Serial.print("currX: ");
  // Serial.print(currX, 5);
  // Serial.print(", currY: ");
  // Serial.print(currY, 5);
  // Serial.print("\n");

  // Serial.print("timeCOUNT: ");
  // Serial.print(timeCOUNT);
  // Serial.print("\n\n");


  delay(delaytime_loop);

  timeEND = millis();
}


// void loop(){

// }
















