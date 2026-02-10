#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>  
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <cmath>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>


// ==================== SERVO CONTROLLER ====================
// class ServoController{
// private:
//   static const size_t GPIO_PIN_SERVO_A = 3;
//   static const size_t GPIO_PIN_SERVO_B = 11;

//   static const int A_ANGLE_INIT = 10;
//   static const int B_ANGLE_INIT = 100;
//   static const int A_ANGLE_CONSTRAIN_MIN = A_ANGLE_INIT - 20;
//   static const int A_ANGLE_CONSTRAIN_MAX = A_ANGLE_INIT + 20;
//   static const int B_ANGLE_CONSTRAIN_MIN = B_ANGLE_INIT - 20;
//   static const int B_ANGLE_CONSTRAIN_MAX = B_ANGLE_INIT + 20;
  
//   Servo servoA;
//   Servo servoB;

// public:
//   void setup(){
//     servoA.attach(GPIO_PIN_SERVO_A, 500, 2500);
//     servoB.attach(GPIO_PIN_SERVO_B, 500, 2500);

//     servoA.write(A_ANGLE_INIT);
//     servoB.write(B_ANGLE_INIT);
//     delay(3000);

//     // servoA.write(constrain(A_ANGLE_INIT+20, A_ANGLE_CONSTRAIN_MIN, A_ANGLE_CONSTRAIN_MAX));
//     // delay(500);
//     // servoA.write(constrain(A_ANGLE_INIT-20, A_ANGLE_CONSTRAIN_MIN, A_ANGLE_CONSTRAIN_MAX));
//     // delay(500);
//     // servoA.write(constrain(A_ANGLE_INIT, A_ANGLE_CONSTRAIN_MIN, A_ANGLE_CONSTRAIN_MAX));
//     // delay(500);
//     // servoB.write(B_ANGLE_INIT+20);
//     // delay(500);
//     // servoB.write(B_ANGLE_INIT-20);
//     // delay(500);
//     // servoB.write(B_ANGLE_INIT);
//     // delay(500);

//   }

//   // void loop(){
//   //   for(size_t i = 0; i < 21; i++){
//   //     servoA.write(A_ANGLE_INIT+i);
//   //     delay(25);
//   //   }
//   //   for(size_t j = 0; j < 21; j++){
//   //     servoA.write(A_ANGLE_INIT+20-j);
//   //     delay(25);
//   //   }
//   //   for(size_t i = 0; i < 21; i++){
//   //     servoB.write(B_ANGLE_INIT+i);
//   //     delay(25);
//   //   }
//   //   for(size_t j = 0; j < 21; j++){
//   //     servoB.write(B_ANGLE_INIT+20-j);
//   //     delay(25);
//   //   }
//   // }
  
// };

// ==================== WIFI-MANAGER ========================
class WiFiManager{
private:
  const char* WIFI_SSID;
  const char* WIFI_PW;
  AsyncWebSocket& ws;
  AsyncWebServer& server;

public:
  WiFiManager(AsyncWebSocket& socket, AsyncWebServer& webserver)
   : WIFI_SSID("A1-D153417A_EXT"), WIFI_PW("FgRnfQhJKVJW7m"),
     ws(socket), server(webserver){}

 void setupWiFi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PW);
    Serial.println("");
    Serial.print("awaiting wifi connection");

    // wait for connection
    while(WiFi.status() != WL_CONNECTED){
      delay(100);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("connected with ");
    Serial.println(WIFI_SSID);
    Serial.print("ip address: \n");
    Serial.print(WiFi.localIP());
 }

  void setupServer(){
    if (!SPIFFS.begin(true)){ 
      Serial.println("SPIFFS failure");
      delay(1000);
      ESP.restart(); 
    }
    // WebSocket
    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_CONNECT) {
        Serial.printf("Client #%u connected\n", client->id());
        client->text("{\"status\":\"connected\"}");
      }
    });

    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      if(SPIFFS.exists("/index.html")){
        request->send(SPIFFS, "/index.html");
      }
      else{
        request->send(404, "text/plain", "index.html not found");
      }
    });
    server.serveStatic("/", SPIFFS, "/"); // every file in data/ is reachable

    ElegantOTA.begin(&server);

    server.begin();
    Serial.println("HTTP server started");
  }

  void loop(){
    ElegantOTA.loop();
    ws.cleanupClients();
  }
};

// ==================== SENSOR DATA =========================
class SensorData{
private:
  sensors_event_t a, g, temp;
  Adafruit_MPU6050& mpu;      // ← pointer to sensor
  AsyncWebSocket&   ws;       // ← pointer to websocket
public:
  SensorData(Adafruit_MPU6050& sensor, AsyncWebSocket& socket)
    : mpu(sensor), ws(socket){}
  void setup(){
    if(!mpu.begin()){
      delay(1000);
      ESP.restart();
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_184_HZ); // 260 deactivates filter, lower freq for more filtering
    // mpu->setCycleRate(MPU6050_CYCLE_40_HZ); // FIXME: cyclerate of 40 clashes with update rate??
  }

  void acquireAndSendData(){
    mpu.getEvent(&a, &g, &temp);

    JsonDocument doc;
    doc["ax"] = a.acceleration.x / 9.81;
    doc["ay"] = a.acceleration.y / 9.81;
    doc["az"] = a.acceleration.z / 9.81;
    doc["gx"] = g.gyro.x;
    doc["gy"] = g.gyro.y;
    doc["gz"] = g.gyro.z;

    String json;
    serializeJson(doc, json);
    ws.textAll(json);
    Serial.println("Sende: " + json);
  }
};

// ==================== GLOBALS =============================
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Adafruit_MPU6050 mpu;
// ServoController ServoController_I1;
WiFiManager WiFiManager_I1(ws, server);
SensorData SensorData_I1(mpu, ws);

// ==================== SETUP ===============================
void setup(){
  Serial.begin(115200);
  Serial.println("---setup finished---");
  delay(1000);
  // ServoController_I1.setup();
  SensorData_I1.setup();
  WiFiManager_I1.setupWiFi();
  WiFiManager_I1.setupServer();
  Serial.println("---setup finished---");

  xTaskCreatePinnedToCore(
    [](void* p){
      auto& s = *static_cast<SensorData*>(p);
      for(;;){
        s.acquireAndSendData();
        vTaskDelay(pdMS_TO_TICKS(10)); // ← 10 ms delay
      }
    },
    "Sensor", 4096, &SensorData_I1, 1, nullptr, 1
  );
}

// ==================== LOOP ================================
void loop(){
  // ServoController_I1.loop();
}
