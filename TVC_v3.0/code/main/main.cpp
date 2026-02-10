#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>


class ServoController{
private:
  static const size_t GPIO_PIN_SERVO_A = 4;
  static const size_t GPIO_PIN_SERVO_B = 5;
  static const size_t GPIO_PIN_SERVO_C = 15; // not finalized -> dont use

  static const size_t A_ANGLE_INIT = 70; // not finalized -> dont use
  static const size_t B_ANGLE_INIT = 70; // not finalized -> dont use
  static const size_t C_ANGLE_INIT = 70; // not finalized -> dont use

  Servo servoA;
  Servo servoB;
  Servo servoC;

public:
  void setup(){
    servoA.attach(GPIO_PIN_SERVO_A);
    servoB.attach(GPIO_PIN_SERVO_B);
    servoC.attach(GPIO_PIN_SERVO_C);

    servoA.write(A_ANGLE_INIT);
    servoB.write(B_ANGLE_INIT);
    servoC.write(C_ANGLE_INIT);
  }
};


class LEDIndicator{
private:
  static const size_t BLINK_PIN = 2;

public:
  static void setup(){
    pinMode(BLINK_PIN, OUTPUT);
  }

  static void system_startup_finish(){
    for(size_t i = 0; i < 9; i++){
      digitalWrite(BLINK_PIN, HIGH);
      delay(200);
      digitalWrite(BLINK_PIN, LOW);
      delay(200);
    }
  }

  static void wifi_status_awaiting(){
    digitalWrite(BLINK_PIN, HIGH);
    delay(70);
    digitalWrite(BLINK_PIN, LOW);
    delay(20);
    digitalWrite(BLINK_PIN, HIGH);
    delay(70);
    digitalWrite(BLINK_PIN, LOW);
    delay(1000);
  }

  static void blinkLoop(){
    digitalWrite(BLINK_PIN, HIGH);
    delay(500);
    digitalWrite(BLINK_PIN, LOW);
    delay(500);
  }
};


class WiFiManager{
private:
  const char* WIFI_SSID;
  const char* WIFI_PW;
  AsyncWebServer server;
  unsigned long ota_progress_millis;

public:
  WiFiManager() : server(80), ota_progress_millis(0), WIFI_SSID("A1-D153417A_EXT"), WIFI_PW("FgRnfQhJKVJW7m"){}

  void setupWiFi(){
    LEDIndicator::setup();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PW);
    Serial.println("");

    // wait for connection
    while(WiFi.status() != WL_CONNECTED){
      LEDIndicator::wifi_status_awaiting();
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("connected with ");
    Serial.println(WIFI_SSID);
    Serial.print("ip address: ");
    Serial.print(WiFi.localIP());
  }

  void setupServer(){
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hi! This is your server.");
    });

    ElegantOTA.begin(&server);

    server.begin();
    Serial.println("HTTP server started");
  }

  void loop(){
    ElegantOTA.loop();
  }
};


struct StateData{ // struct to temp save one state
  float ax, ay, az;
  float gx, gy, gz;
  float vel_x, vel_y, vel_z;
  float pos_x, pos_y, pos_z;
  float yaw, pitch, roll; // around x, around y, around z
  float times;
};


class WhereAmI{
private:
  static const int SDA_PIN = 21;
  static const int SCL_PIN = 22;
  static const float COMP_FILTER_FAC = 0.98; // 98% gyro, 2% accelerometer
  static const int SAMPLES = 5;
  static const float EARTH_ACC = 9.81;
  bool first_loop = true;


  StateData currentState;
  Adafruit_MPU6050 mpu;

public:
  WhereAmI(){
    currentState = {0.0, 0.0, 0.0, // ax, ay, az
                    0.0, 0.0, 0.0, // gx, gy, gz
                    0.0, 0.0, 0.0, // vel_x, vel_y, vel_z
                    0.0, 0.0, 0.0, // pos_x, pos_y, pos_z
                    0.0, 0.0, 0.0, // pitch, yaw, roll  // around x, around y, around z
                    0.0};          // times
    }
  
  void setup(){
    Wire.begin(SDA_PIN, SCL_PIN);
    if(!mpu.begin()){
      Serial.println("MPU6050 not found! -restart-");
      delay(2000);
      ESP.restart(); // ??
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_184_HZ); // 260 deactivates filter, lower freq for more filtering
    mpu.setCycleRate(MPU6050_CYCLE_40_HZ); // FIXME: cyclerate of 40 clashes with update rate??
  }

  StateData update_state(StateData exData){
    sensors_event_t accel, gyro, temp;
    float ax[SAMPLES+1] = {0.0}, ay[SAMPLES+1] = {0.0}, az[SAMPLES+1] = {0.0};
    float gx[SAMPLES+1] = {0.0}, gy[SAMPLES+1] = {0.0}, gz[SAMPLES+1] = {0.0};
    float vel_x[SAMPLES] = {0.0}, vel_y[SAMPLES] = {0.0}, vel_z[SAMPLES] = {0.0};
    float pos_x = 0.0, pos_y = 0.0, pos_z = 0.0;
    float g_pitch = 0.0, g_yaw = 0.0, g_roll = 0.0;
    float acc_pitch = 0.0, acc_yaw = 0.0, acc_roll = 0.0;
    float yaw = 0.0, pitch = 0.0, roll = 0.0;
    float times[SAMPLES+1] = {0.0};
    // float temperature;


    // getting data
    for(size_t i = first_loop ? 0 : 1; i < SAMPLES + 1; i++){
      mpu.getEvent(&accel, &gyro, &temp);
      times[i] = micros() / 1000000.0;
      ax[i] = accel.acceleration.x;
      ay[i] = accel.acceleration.y;
      az[i] = accel.acceleration.z;
      gx[i] = gyro.gyro.x;
      gy[i] = gyro.gyro.y;
      gz[i] = gyro.gyro.z;
    }
  

    // calculating velocities and orientation
    if(!first_loop){
      vel_x[0] = exData.vel_x + (exData.ax+ax[1])*0.5*(times[1]-exData.times);
      vel_y[0] = exData.vel_y + (exData.ay+ay[1])*0.5*(times[1]-exData.times);
      vel_z[0] = exData.vel_z + (exData.az+az[1])*0.5*(times[1]-exData.times); 

      // gyro-part of orientation
      g_pitch = exData.pitch + (exData.gx+gx[1])*0.5*180/M_PI*(times[1]-exData.times);
      g_yaw   = exData.yaw   + (exData.gy+gy[1])*0.5*180/M_PI*(times[1]-exData.times);
      g_roll  = exData.roll  + (exData.gz+gz[1])*0.5*180/M_PI*(times[1]-exData.times);
      
      // accel-part of orientation
      acc_pitch = atan2(-ay[1], az[1])*180/M_PI;
      acc_yaw   = atan2(-ax[1], sqrt(ay[1]*ay[1]+az[1]*az[1]))*180/M_PI;
      // acc_roll  = atan2((exData.ax+ax[1])*0.5, (exData.ay+ay[1])*0.5)*180/M_PI;
    }
    else{ // first loop
      vel_x[0] = (ax[0]+ax[1])*0.5*(times[1]-times[0]);
      vel_y[0] = (ay[0]+ay[1])*0.5*(times[1]-times[0]);
      vel_z[0] = (az[0]+az[1])*0.5*(times[1]-times[0]);

      // gyro-part of orientation
      g_pitch += (gx[0]+gx[1])*0.5*180/M_PI*(times[1]-times[0]);
      g_yaw   += (gy[0]+gy[1])*0.5*180/M_PI*(times[1]-times[0]);
      g_roll  += (gz[0]+gz[1])*0.5*180/M_PI*(times[1]-times[0]);

      // accel-part of orientation
      acc_pitch = atan2(-ay[0], az[0])*180/M_PI;
      acc_yaw   = atan2(-ax[0], sqrt(ay[0]*ay[0]+az[0]*az[0]))*180/M_PI;
      // acc_roll  = atan2((ax[0]+ax[1])*0.5, (ay[0]+ay[1])*0.5)*180/M_PI;
    }
    for(size_t i = 1; i < SAMPLES; i++){ 
      vel_x[i] = vel_x[i-1] + (ax[i]+ax[i+1])*0.5*(times[i+1]-times[i]);
      vel_y[i] = vel_y[i-1] + (ay[i]+ay[i+1])*0.5*(times[i+1]-times[i]);
      vel_z[i] = vel_z[i-1] + (az[i]+az[i+1])*0.5*(times[i+1]-times[i]);

      // gyro-part of orientation
      g_pitch += (gx[i]+gx[i+1])*0.5*180/M_PI*(times[i+1]-times[i]);
      g_yaw   += (gy[i]+gy[i+1])*0.5*180/M_PI*(times[i+1]-times[i]);
      g_roll  += (gz[i]+gz[i+1])*0.5*180/M_PI*(times[i+1]-times[i]);
      
      // accel-part of orientation
      acc_pitch = atan2(-ay[i], az[i])*180/M_PI;
      acc_yaw   = atan2(-ax[i], sqrt(ay[i]*ay[i]+az[i]*az[i]))*180/M_PI;
      // acc_roll  = atan2((ax[0]+ax[1])*0.5, (ay[0]+ay[1])*0.5)*180/M_PI;
    }


    // calculating positions
    if(!first_loop){ 
      pos_x = exData.pos_x + (exData.vel_x+vel_x)*0.5*(times[1]-exData.times);
      pos_y = exData.pos_y + (exData.vel_y+vel_y)*0.5*(times[1]-exData.times);
      pos_z = exData.pos_z + (exData.vel_z+vel_z)*0.5*(times[1]-exData.times);
    }
    for(size_t i = 1; i < SAMPLES; i++){ 
      pos_x += (vel_x[i-1]+vel_x[i])*0.5*(times[i]-times[i-1]);
      pos_y += (vel_y[i-1]+vel_y[i])*0.5*(times[i]-times[i-1]);
      pos_z += (vel_z[i-1]+vel_z[i])*0.5*(times[i]-times[i-1]);
    }


    // filling values for the next iteration
    StateData loopData = {ax[SAMPLES], ay[SAMPLES], az[SAMPLES],
                          gx[SAMPLES], gy[SAMPLES], gz[SAMPLES],
                          vel_x[SAMPLES-1], vel_y[SAMPLES-1], vel_z[SAMPLES-1],
                          pos_x, pos_y, pos_z,
                          pitch, yaw, roll,
                          times[SAMPLES]
    };

    if(first_loop){ first_loop = false; }
    return loopData;
  }
};

// class instantiating
ServoController ServoController_I1;
WiFiManager WiFiManager_I1;
WhereAmI WhereAmI_I1;


void setup(){
  Serial.begin(115200);
  ServoController_I1.setup();
  WiFiManager_I1.setupWiFi();
  WiFiManager_I1.setupServer();
  WhereAmI_I1.setup();
}

void loop(){
// looping WhereAmI_I1 update_state
}



/* 
#### TO-DO ####
unfinished:
  big:
    ~ class WhereAmI
    ~ class servoController w/ or  flight correction 
      + define starting values
      + implement influence of gravity
  
  small:
    ~ check if classes are seperable into new files
    ~ check for possibility to substitute delay() with vTaskDelay()

new:
  big:
    ~ class nominal flight route
    ~ class flight correction
    ~ main setup
    ~ main loop

  small:

*/




































