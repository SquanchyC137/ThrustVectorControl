#include <Adafruit_MPU6050.h>  // Lagesensor (Beschleunigung/Drehrate)
#include <Adafruit_NeoPixel.h> // esp32 RGB-LED control
#include <Adafruit_Sensor.h>   // Basisklasse für Adafruit Sensoren
#include <Arduino.h>           // Arduino Core Funktionen
#include <ArduinoJson.h>       // JSON Datenverarbeitung (Web-Kommunikation)
#include <AsyncTCP.h>          // Basis für asynchrone Netzwerkverbindungen
#include <ESP32Servo.h>        // Steuerung der Servomotoren
#include <ESPAsyncWebServer.h> // Asynchroner Webserver für das Dashboard
#include <ElegantOTA.h>        // Over-the-Air Updates über den Browser
#include <SPIFFS.h> // Filesystem für Dateien im Flash (z.B. index.html)
#include <WiFi.h>   // WiFi-Verbindung
#include <Wire.h>   // I2C-Kommunikation (für den MPU6050)
#include <cmath>    // Mathematische Funktionen (Berechnungen)

// ==================== HARDWARE MAPPING ====================
namespace Hardware {
constexpr int ONBOARD_RGB_BRIGHTNESS = 64;
constexpr int ONBOARD_RGB_NUM_PIXELS = 1;
constexpr int ONBOARD_RGB_PIN = 48;

constexpr int ACTIVE_BUZZER_PIN = 14;
constexpr int PASSIVE_BUZZER_PIN = 13;

constexpr int I2C_SCL_PIN = 6;
constexpr int I2C_SDA_PIN = 4;

constexpr int SERVO_A_PIN = 3;
constexpr int SERVO_B_PIN = 11;
} // namespace Hardware

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

//     // servoA.write(constrain(A_ANGLE_INIT+20, A_ANGLE_CONSTRAIN_MIN,
//     A_ANGLE_CONSTRAIN_MAX));
//     // delay(500);
//     // servoA.write(constrain(A_ANGLE_INIT-20, A_ANGLE_CONSTRAIN_MIN,
//     A_ANGLE_CONSTRAIN_MAX));
//     // delay(500);
//     // servoA.write(constrain(A_ANGLE_INIT, A_ANGLE_CONSTRAIN_MIN,
//     A_ANGLE_CONSTRAIN_MAX));
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
class WiFiManager {
private:
  const char *WIFI_SSID;
  const char *WIFI_PW;
  AsyncWebSocket &ws;
  AsyncWebServer &server;

  // Statische IP Konfiguration
  IPAddress local_IP;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress primaryDNS;   // Optional
  IPAddress secondaryDNS; // Optional

public:
  WiFiManager(AsyncWebSocket &socket, AsyncWebServer &webserver)
      : WIFI_SSID("A1-D153417A_EXT"), WIFI_PW("FgRnfQhJKVJW7m"), ws(socket),
        server(webserver),
        local_IP(192, 168, 1, 200), // <--- Neue IP für den ESP32
        gateway(192, 168, 1, 138),  // <--- Dein Router (laut Screenshot)
        subnet(255, 255, 255, 0),
        primaryDNS(8, 8, 8, 8),  // Optional: Google DNS
        secondaryDNS(8, 8, 4, 4) // Optional: Google DNS
  {}

  void setupWiFi() {
    // Statische IP anwenden, bevor WiFi gestartet wird
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("Static IP configuration failed!");
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PW);
    Serial.println("");
    Serial.print("awaiting wifi connection");

    // wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("connected with ");
    Serial.println(WIFI_SSID);
    Serial.print("ip address: \n");
    Serial.println(WiFi.localIP());
  }

  void setupServer() {
    if (!SPIFFS.begin(true)) {
      Serial.println("SPIFFS failure");
      delay(1000);
      ESP.restart();
    }
    // WebSocket
    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                  AwsEventType type, void *arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_CONNECT) {
        Serial.printf("Client #%u connected\n", client->id());
        client->text("{\"status\":\"connected\"}");
      }
    });

    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (SPIFFS.exists("/index.html")) {
        request->send(SPIFFS, "/index.html");
      } else {
        request->send(404, "text/plain", "index.html not found");
      }
    });
    server.serveStatic("/", SPIFFS, "/"); // every file in data/ is reachable

    ElegantOTA.begin(&server);

    server.begin();
    Serial.println("HTTP server started");
  }

  void loop() {
    ElegantOTA.loop();
    ws.cleanupClients();
  }
};

// ==================== SENSOR DATA =========================
class SensorData {
private:
  sensors_event_t a, g, temp;
  Adafruit_MPU6050 &mpu; // ← pointer to sensor
  AsyncWebSocket &ws;    // ← pointer to websocket
public:
  SensorData(Adafruit_MPU6050 &sensor, AsyncWebSocket &socket)
      : mpu(sensor), ws(socket) {}
  void setup() {
    if (!mpu.begin()) {
      delay(1000);
      Serial.println("MPU6050 failure, retrying...");
      ESP.restart();
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(
        MPU6050_BAND_184_HZ); // 260 deactivates filter, lower freq for more
                              // filtering
    // mpu->setCycleRate(MPU6050_CYCLE_40_HZ); // FIXME: cyclerate of 40 clashes
    // with update rate??
  }

  void acquireAndSendData() {
    mpu.getEvent(&a, &g, &temp);

    JsonDocument doc;
    doc["ax"] = a.acceleration.x; // / 9.81;
    doc["ay"] = a.acceleration.y; // / 9.81;
    doc["az"] = a.acceleration.z; // / 9.81;
    doc["gx"] = g.gyro.x;
    doc["gy"] = g.gyro.y;
    doc["gz"] = g.gyro.z;
    doc["t"] = temp.temperature;

    String json;
    serializeJson(doc, json);
    ws.textAll(json);
    // Serial.println("Sende: " + json);
  }
};

// ==================== GLOBALS =============================
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Adafruit_MPU6050 mpu;
// ServoController ServoController_I1;
WiFiManager WiFiManager_I1(ws, server);
SensorData SensorData_I1(mpu, ws);
Adafruit_NeoPixel pixel(Hardware::ONBOARD_RGB_NUM_PIXELS,
                        Hardware::ONBOARD_RGB_PIN, NEO_GRB + NEO_KHZ800);

// ================== buzzer func ===========================
void buzzerSound(int frequency, int duration, bool activeBuzzer, int num) {
  if (activeBuzzer == 1) {
    for (int i = 0; i < num; i++) {
      digitalWrite(Hardware::ACTIVE_BUZZER_PIN, HIGH);
      delay(duration);
      digitalWrite(Hardware::ACTIVE_BUZZER_PIN, LOW);
      delay(100);
    }
  } else {
    for (int i = 0; i < num; i++) {
      tone(Hardware::PASSIVE_BUZZER_PIN, frequency, duration);
      delay(duration + 100);
    }
  }
}

// ==================== RGB func ============================
void rgb_blink() {
  Serial.println("LED Blau");
  pixel.setPixelColor(0, pixel.Color(0, 0, 255));
  pixel.show();
  delay(1000);
  Serial.println("LED Rot");
  pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  pixel.show();
  delay(300);
  Serial.println("LED Blau");
  pixel.setPixelColor(0, pixel.Color(0, 0, 255));
  pixel.show();
  delay(1000);
  Serial.println("LED Aus");
  pixel.setPixelColor(0, pixel.Color(0, 0, 0));
  pixel.show();
}

// ==================== SETUP ===============================
void setup() {
  Serial.begin(115200);
  Serial.println("---setup started---");
  // RGB LED Initialisierung
  pixel.begin();
  pixel.setBrightness(Hardware::ONBOARD_RGB_BRIGHTNESS);
  rgb_blink();
  rgb_blink();
  delay(1000);

  pinMode(Hardware::ACTIVE_BUZZER_PIN, OUTPUT);
  pinMode(Hardware::PASSIVE_BUZZER_PIN, OUTPUT);
  digitalWrite(Hardware::ACTIVE_BUZZER_PIN, LOW);
  digitalWrite(Hardware::PASSIVE_BUZZER_PIN, LOW);

  buzzerSound(3000, 150, 0,
              1); // frequency, duration(ms), activeBuzzer?, num repeats

  // I2C Initialisierung mit SDA und SCL
  Wire.begin(Hardware::I2C_SDA_PIN, Hardware::I2C_SCL_PIN);

  delay(1000);
  // ServoController_I1.setup();
  SensorData_I1.setup();
  Serial.println("Sensor-Data setup done");
  WiFiManager_I1.setupWiFi();
  Serial.println("WiFiManager WiFi setup done");
  WiFiManager_I1.setupServer();
  Serial.println("WiFiManager Server setup done");

  xTaskCreatePinnedToCore(
      [](void *p) {
        auto &s = *static_cast<SensorData *>(p);
        for (;;) {
          s.acquireAndSendData();
          vTaskDelay(pdMS_TO_TICKS(100)); // ← xxx ms delay
        }
      },
      "Sensor", 4096, &SensorData_I1, 1, nullptr, 1);
}

// ==================== LOOP ================================
void loop() {
  // Handelt OTA Updates und WebSocket Aufräumarbeiten
  WiFiManager_I1.loop();
  // ServoController_I1.loop();
}
