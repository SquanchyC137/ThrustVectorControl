#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ESP32Servo.h>
#include "globals.h"


// OTA includes
#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>


Servo servoX;
Servo servoY;


//TaskHandle_t Servo1TaskHandle;
//TaskHandle_t Servo2TaskHandle;


// void ServoXinit(void *pvParameters){
//   servoX.attach(GPIO_pin_servoX);

//   // for ( int i = 0; i < 10; i++){
//   //   servoX.write(i);
//   //   Serial.println(servoX.read());
//   //   vTaskDelay(1*1000/portTICK_PERIOD_MS);
//   // }

//   servoX.write(X_angle_init);
//   // servoX.detach();
//   vTaskDelete(NULL);
// }


// void ServoYinit(void *pvParameters){
//   servoY.attach(GPIO_pin_servoY);
//   servoY.write(Y_angle_init);
//   servoY.detach();
//   vTaskDelete(NULL);
// }






















void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  // servoX.attach(GPIO_pin_servoX);
  servoY.attach(GPIO_pin_servoY);
  
  servoY.write(Y_angle_init);
  // servoX.write(X_angle_init);
  // delay(1000);
  // servoX.detach();

  // xTaskCreatePinnedToCore(ServoXinit, "ServoX init", 2048, NULL, 1, NULL, 0); // Core 0
  // xTaskCreatePinnedToCore(ServoYinit, "ServoY init", 2048, NULL, 1, NULL, 1); // Core 1
  
  // Serial.print("Servos initialisiert.");
}

void loop() {


  ArduinoOTA.handle();

  
  for( int i = 0; i < 15; i++){
    servoY.write(i+Y_angle_init);
    delay(10);
  }
  delay(500);
  for ( int j = 15; j > 0; j--){
    servoY.write(j+Y_angle_init);
    delay(10);
  }
  delay(500);

}







