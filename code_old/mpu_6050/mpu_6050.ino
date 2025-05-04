#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

#define conval 0.0174533   // rad->deg | 1 deg = pi/180 = 0.0174532

Adafruit_MPU6050 mpu;
Servo servo1;
Servo servo2;

const double delaytime = 100;
const double pos1 = 65;
const double pos2 = 135;
int _init = 1;  
double currX, currY = 0;
double gx, gy = 0;
unsigned long timeSTART, timeEND, timeDIFF, timeCOUNT = 0;


void setup() {
  Serial.begin(9600);
  mpu.begin();

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  servo2.attach(9);  // turns around x - axis
  servo1.attach(10); // turns around y - axis
}


void correctX(double currX){
  servo2.write(currX);
}

void correctY(double currY){
  servo1.write(currY);
}

double toleranceFilter(double g){
  if(abs(g) < 0.2){
    g = 0;
  }
  return g;
}


void loop() {
  // initiate default servo position
  if(_init){
    servo1.write(pos1);
    servo2.write(pos2);
    currX = pos2;
    currY = pos1;
    _init = 0;
  }

  // eval intervals to correct xy-positions
  timeDIFF = timeEND - timeSTART;
  timeCOUNT += timeDIFF;
  if(timeCOUNT >= 500){
    currX += gx*(timeCOUNT/1000);
    currY += gy*(timeCOUNT/1000);
    correctX(currX);
    correctY(currY);
    timeCOUNT = 0;
  }

  timeSTART = millis();


  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);


  gx = (g.gyro.x / conval)+3.12;
  gx = toleranceFilter(gx);
  gy = (g.gyro.y / conval)-1.331;
  gy = toleranceFilter(gy);




  Serial.print("gx: ");
  Serial.print(gx, 3);
  Serial.print(", gy: ");
  Serial.print(gy, 3);

  Serial.print("\n");

  Serial.print("currX: ");
  Serial.print(currX, 5);
  Serial.print(", currY: ");
  Serial.print(currY, 5);
  Serial.print("\n\n");


  // correctX(currX);
  // correctY(currY);


  delay(delaytime);

  timeEND = millis();
}




















