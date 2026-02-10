#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>  
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <cmath>
#include <ArduinoJson.h>


// class Blink{
// private:
//   static const int BLINK_PIN = 2;

// public:
//   static void setup(){
//     pinMode(BLINK_PIN, OUTPUT);
//   }

//   static void startup(){
//     for(size_t i = 0; i < 5; i++){
//       digitalWrite(BLINK_PIN, HIGH);
//       delay(200);
//       digitalWrite(BLINK_PIN, LOW);
//       delay(200);
//     }
//   }
// };

class ServoController{
private:
  static const size_t GPIO_PIN_SERVO_A = 3;
  static const size_t GPIO_PIN_SERVO_B = 11;

  static const size_t A_ANGLE_INIT = 20; 
  static const size_t B_ANGLE_INIT = 100; 
  Servo servoA;
  Servo servoB;

public:
  void setup(){
    servoA.attach(GPIO_PIN_SERVO_A);
    servoB.attach(GPIO_PIN_SERVO_B);

    servoA.write(A_ANGLE_INIT);
    delay(10);
    servoB.write(B_ANGLE_INIT);
    delay(2000);
  }

  // void loop(){
  //   for(size_t i = 0; i < 81; i++){
  //     servoA.write(A_ANGLE_INIT+(i*0.25));
  //     servoB.write(B_ANGLE_INIT+(i*0.25));
  //     delay(10);
  //   }
  //   for(size_t j = 0; j < 161; j++){
  //     servoA.write(A_ANGLE_INIT+20-(j*0.25));
  //     servoB.write(B_ANGLE_INIT+20-(j*0.25));
  //     delay(10);
  //   }
  //     for(size_t i = 0; i < 81; i++){
  //     servoA.write(A_ANGLE_INIT-20+(i*0.25));
  //     servoB.write(B_ANGLE_INIT-20+(i*0.25));
  //     delay(10);
  //   }
  // }
};


// class WiFiManager{
// private:
//  const char* WIFI_SSID;
//  const char* WIFI_PW;
//  AsyncWebServer server;

// public:
//  WiFiManager() : server(80), WIFI_SSID("A1-D153417A"), WIFI_PW("FgRnfQhJKVJW7m"){}

//  void setupWiFi(){
//    WiFi.mode(WIFI_STA);
//    WiFi.begin(WIFI_SSID, WIFI_PW);
//    Serial.println("");

//    // wait for connection
//    while(WiFi.status() != WL_CONNECTED){
//      Serial.print(".");
//    }
//    Serial.println("");
//    Serial.print("connected with ");
//    Serial.println(WIFI_SSID);
//    Serial.print("ip address: ");
//    Serial.print(WiFi.localIP());
//  }

//  void setupServer(){
//    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//      request->send(200, "text/plain", "Hi! This is your server.");
//    });

//    ElegantOTA.begin(&server);
//    ElegantOTA.onStart(onOTAStart);
//    ElegantOTA.onProgress(onOTAProgress);
//    ElegantOTA.onEnd(onOTAEnd);

//    server.begin();
//    Serial.println("HTTP server started");
//  }

//  void loop(){
//    ElegantOTA.loop();
//  }
 
//  static void onOTAStart(){
//    // Log when OTA has started
//    Serial.println("OTA update started!");
//    // <Add more code if needed>
//  }

//  static void onOTAProgress(size_t current, size_t final){
//    static unsigned long ota_progress_millis = 0;
//    // Log every 1 second
//    if(millis() - ota_progress_millis > 1000){
//      ota_progress_millis = millis();
//      Serial.printf("OTA PROGRESS CURRENT: %u bytes, Final: %u bytes\n", current, final);
//    }
//  }

//  static void onOTAEnd(bool success){
//    // Log when OTA has finished
//    if(success){
//      Serial.println("OTA update finished successfully!");
//    }
//    else{
//      Serial.println("There was an error during OTA update!");
//    }
//  }
// };

// class instantiating
ServoController ServoController_I1;
// WiFiManager WiFiManager_I1;


void setup(){
  Serial.begin(115200);
  // Blink::setup;
  // Blink::startup;
  ServoController_I1.setup();
  // WiFiManager_I1.setupWiFi();
  // WiFiManager_I1.setupServer();


}

void loop(){
  // ServoController_I1.loop();
}
