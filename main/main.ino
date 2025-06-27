#include "BluetoothSerial.h"
#include <WiFi.h>
#include <ArduinoJson.h>  // Needed to parse JSON-formatted Bluetooth commands
#include <Preferences.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// MPU6050 libraries and variables
#include <Wire.h>

const int MPU_ADDR = 0x68;  // MPU6050 I2C address
// const int LED_PIN = 15;     // GPIO pin connected to the LED
int16_t acc_x_raw, acc_y_raw, acc_z_raw;
// Calibrated values
float acc_x, acc_y, acc_z;
// Calibration offsets
float acc_x_offset = 0.0;
float acc_y_offset = 0.0;
float acc_z_offset = 0.0;
// Fall detection thresholds
float impact_threshold = 2.5;        // g-force for impact
float low_activity_threshold = 0.3;  // g-force for post-impact stillness
unsigned long last_fall_time = 0;
unsigned long fall_timeout = 3000;   // 3 seconds between fall events

bool fall_detected = false; // Flag to keep LED on after fall
unsigned long last_sample_time = 0;
const unsigned long sample_interval = 20; // milliseconds (50Hz)
// MPU6050 libraries and variables


//############### wifi bluetooth
BluetoothSerial SerialBT;
Preferences prefs;

SemaphoreHandle_t wifiMutex;
volatile bool pauseMonitor = false;
volatile bool isUser_atHome = true;

// ###############  GPS
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600
#define GPS_TIMEOUT 300000

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);


struct GPSLocation {
  double latitude;
  double longitude;
};

// GSM serial
HardwareSerial gsm(1); // Use UART1 (pins 16, 17)

// ********************************* setup ************
void setup() {
  Serial.begin(115200);
  prefs.begin("wifi_prefs", false);
  wifiMutex = xSemaphoreCreateMutex();
  xTaskCreate(Zone_monitor, "ZoneMonitor", 4096, NULL, 1, NULL);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  SerialBT.begin("ESP32_BT_Device"); // Bluetooth device name
  Serial.println("Bluetooth started!");
  WiFi.setAutoReconnect(false);

  Wire.begin();
  Wire.setClock(400000);  // Fast I2C speed
  // pinMode(LED_PIN, OUTPUT);
  // digitalWrite(LED_PIN, LOW);  // LED off initially
  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);  // Set sleep = 0
  Wire.endTransmission(true);
  // Calibrate offsets
  calibrateAccelerometer();
  Serial.println("Fall Detection Initialized.");

  xTaskCreate(detect_falls,"detect_falls", 2048, NULL, 1 , NULL);
  size_t freeStack = uxTaskGetStackHighWaterMark(NULL);  // NULL gets the stack usage of the current task
  Serial.print("freeStack: ");
  Serial.println(freeStack);
}

void Zone_monitor(void *pvParameters) {
  while (1) {
    if (pauseMonitor) {
      // If paused, skip this cycle (but not in the middle of one)
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    if (xSemaphoreTake(wifiMutex, portMAX_DELAY)) {
      Serial.println("Zone monitoring...");
      if (isUser_atHome){
        monitorZone();
      } else {
        trackUser();
      }
      xSemaphoreGive(wifiMutex);
    }

    if (isUser_atHome){
      vTaskDelay(3000 / portTICK_PERIOD_MS);  // monitorZone() function delay
    } else {
      vTaskDelay(1000*20 / portTICK_PERIOD_MS);  // trackUser() function delay
    }
  }
}


void loop() {
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n'); // Read till newline for safer JSON parsing
    Serial.println("Received: " + incoming);
    SerialBT.println("Echo: " + incoming);
    handle_bluetooth_command(incoming);
  }
}
