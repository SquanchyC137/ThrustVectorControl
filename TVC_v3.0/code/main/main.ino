#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ESP32Servo.h>
#include <math.h>

// #include <Interval.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


#include "globals.h"

Adafruit_MPU6050 mpu;

Servo servoA;
Servo servoB;
Servo servoC;

AsyncWebServer server(80);

unsigned long ota_progress_millis = 0;


float compFilter_fac = 0.98;
float oldXangle, oldYangle = 0;
float newXangle, newYangle = 0;





void wifiSetup(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PW);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void serverSetup(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is ElegantOTA AsyncDemo.");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.println("HTTP server started");
}


void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}


void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}


void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
    // ESP.restart();
  } else {
    Serial.println("There was an error during OTA update!");
  }
}


void blinkFinish(){
  pinMode(BLINK_PIN, OUTPUT);

    digitalWrite(BLINK_PIN, HIGH);
    delay(5000);
    digitalWrite(BLINK_PIN, LOW);
  // for( int i = 0; i < 3; i++){
  //   digitalWrite(BLINK_PIN, HIGH);
  //   delay(500);
  //   digitalWrite(BLINK_PIN, LOW);
  //   delay(500);
  // }
  delay(5000);
}


void servoSetup(){
  servoaA.attach(GPIO_pin_servoA);
  // servoB.attach(GPIO_pin_servoB);
  // servoC.attach(GPIO_pin_servoC);

  // servoA.write(A_angle_init);
  // servoB.write(B_angle_init);
  // servoC.write(C_angle_init);
}


// void mpuSetup(){
//   // pinMode(SLA_PIN, INPUT_PULLUP);
//   // pinMode(SCL_PIN, INPUT_PULLUP);
//   // pinMode(INT_PIN, INPUT);

//   if(!mpu.begin()){
//     Serial.println("MPU not found ");
//     abort();
//   }

//   mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//   mpu.setGyroRange(MPU6050_RANGE_500_G);
//   mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
// }

// void mpuLoop(void *parameter){
//   static float accelX, accelY, accelZ;
//   static float gyroX, gyroY, gyroZ;

//   static gyroXangle_sum, gyroYangle_sum = 0;

//   static unsigned long mpu_deltaT, servo_deltaT;
//   static unsigned long mpu_timer_prev, mpu_timer_curr = 0;
  
  

//   sensors_event_t a, g, t; // acceleration, gyro, and temperature
//   mpu.getEvent(&a, &g, &t);


//   //measuring time between each datapoint
//   mpu_timer_curr = micros();
//   mpu_deltaT = mpu_timer_curr - mpu_timer_prev;
//   servo_deltaT += mpu_deltaT;
//   mpu_timer_prev = mpu_timer_curr;

//   accelX = a.acceleration.x;
//   accelY = a.acceleration.y;
//   accelZ = a.acceleration.z;

//   gyroX = g.gyro.x;
//   gyroY = g.gyro.y;
//   gyroZ = g.gyro.z;

//   temp = t.temperature;


//   //get filtered x-orientation / pitch
//   gyroXangle = gyroX * mpu_deltaT;
//   accelXangle = arctan(accelY / accelZ) * (180 / M_PI);
//   newXangle = compFilter_fac * (oldXangle + gyroXangle) + (1 - compFilter_fac) * accelXangle;
//   oldXangle += newXangle;

//   //get filtered y-orientation / roll
//   gyroYangle = gyroY * mpu_deltaT;
//   accelYangle = arctan(accelX / accelZ) * (180 / M_PI);
//   newYangle = compFilter_fac * (oldYangle + gyroYangle) + (1 - compFilter_fac) * accelYangle;
//   oldYangle = newYangle;

//   if (servo_deltaT > 20e3){


//     servo_deltaT = 0;
//   }


// }

// void servoCorrect(){

// }



void setup(void) {
  Serial.begin(115200);
 
  wifiSetup();
  serverSetup();
  
  servoSetup();

  /// everything set up
  blinkFinish();

  // xTaskCreatePinnedToCore(mpuLoop, "MPU Loop", 2048, NULL, 1, NULL, 0);

}

void loop(void) {
  ElegantOTA.loop();

  digitalWrite(BLINK_PIN, HIGH);
  delay(1000);
  digitalWrite(BLINK_PIN, LOW);
  delay(1000);
}








