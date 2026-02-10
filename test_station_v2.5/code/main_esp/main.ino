#include <Wifi.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>  
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
// #include <math.h>
#include <ArduinoJson.h>


class ServoController{
private:
  static const size_t GPIO_PIN_SERVO_A = 35;
  static const size_t GPIO_PIN_SERVO_B = 36;

  static const size_t A_ANGLE_INIT = 70; // not finalized -> dont use
  static const size_t B_ANGLE_INIT = 70; // not finalized -> dont use

  Servo servoA;
  Servo servoB;

public:
  void setup(){
    servoA.attach(GPIO_PIN_SERVO_A);
    servoB.attach(GPIO_PIN_SERVO_B);

    servoA.write(A_ANGLE_INIT);
    servoB.write(B_ANGLE_INIT);
  }
};


class WiFiManager{ 
private:
  static const char WIFI_SSID[] = "A1-D153417A";
  static const char WIFI_PW[] = "FgRnfQhJKVJW7m";
  AsyncWebServer server;
  unsigned long ota_progress_millis;

public:
  WiFiManager() : server(80), ota_progress_millis(0){}

  void setupWiFi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PW);
    Serial.println("");

    // wait for connection
    while(WiFi.status() != WL_CONNECTED){
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
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    server.begin();
    Serial.println("HTTP server started");
  }

  void loop(){
    ElegantOTA.loop();
  }
  
  void onOTAStart(){
    // Log when OTA has started
    Serial.println("OTA update started!");
    // <Add more code if needed>
  }

  void onOTAProgress(size_t current, size_t final){
    // Log every 1 second
    if(millis() - ota_progress_millis > 1000){
      ota_progress_millis = millis();
      Serial.printf("OTA PROGRESS CURRENT: %u bytes, Final: %u bytes\n", current, final);
    }
  }

  void onOTAEnd(bool success){
    // Log when OTA has finished
    if(success){
      Serial.println("OTA update finished successfully!");
    }
    else{
      Serial.println("There was an error during OTA update!");
    }
  }
};

// class instantiating
ServoController ServoController_I1;
WiFiManager WiFiManager_I1;


void setup(){
  Serial.begin(115200);
  WiFiManager_I1.setupWiFi();
  WiFiManager_I1.setupServer();
  ServoController_I1.setup();

}

void loop(){

}